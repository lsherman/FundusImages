clear all;
close all;
clc;
load DRIVE_trn_n;
%% This is the 2nd code to be run for STARE test images
%% First set all the paths to the images %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
STARE_configure_rework;

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
imfiles      = textread(roi.evalfiles,'%s','delimiter','\n');
numtest     = size(imfiles,1);
maskfiles=textread('Data\STARE\data.txt','%s','delimiter','\n');


%% Next begin to test each image for vessel segmentation using the DRIVE train set of images

%% Begin with test images-----------------------------------------------------------
for j=11
    clear P Rec images im_top
    tst_ind=1;
     fprintf('\r Testing image number= %d',j);
     evalfile = deblank(imfiles{j});
     I = imread([roi.evalfilepath{1} evalfile(:,1:end) '.jpg']);
     K=(I(:,:,2));
    [u,v]=size(K);
    
    %% Define the background masks     
     maskfile=deblank(maskfiles{j});
     g4=imread(['Data\STARE\FOVMasks\' maskfile]);
     ge=im2double(g4)>0.9;
     g=imresize(ge,[u v]);
    
     g3=imread('Data\DRIVE_second.jpg');
     g0=im2double(g3(:,:,2))>0.9;
     g0=imresize(g0,[u v],'nearest');
     Y=regionprops(g0,'PixelList');
     pixg=cat(1,Y.PixelList);
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%     
tstart=tic;
H=im2double(K);
HH0=imadjust(H);
%% High pass filter image extraction (H=I_sc3)
 w=fspecial('average',[20 20]);
 I_bg=(imfilter(HH0,w,'replicate'));
I_sc=HH0-I_bg;
I_fore=I_sc>0.05;
[r,c]=find(I_sc>=0.0);
for i=1:size(r,1)
    I_sc(r(i),c(i))=0;
end
I_sc1=abs(I_sc);
I_sc3=imadjust(I_sc1.*g);
I_sc3=enhancer_new(I_sc3,1);

%% Extraction of morohologically tophat recosntructed image (T=Kn1)
I_neg=(1-HH0).*g;
U=enhancer(imadjust(I_neg),2);
for h=1:12
    se=strel('line',11,(h-1)*15);
    %se=strel('disk',(h-1)*15);
    im_top=imtophat(U,se);
    images(h,:)=reshape(im_top,1,u*v);
end
max_top=max(images,[],1);
Kn1=reshape(max_top,u,v);
Kn1=imadjust(enhancer(Kn1,2));

%% Extraction of two binary images, (H'=Kn2, T'=I_sc4)
Kn2=Kn1>0.1;
Kn2=bwareaopen(Kn2,30);
I_sc4=I_sc3>0.1;
I_sc4=bwareaopen(I_sc4,30);

%% Finding the intersecting regions in the two binary images as the major vessels (P=V1=H' intersect T')
P=bwareaopen(Kn2.*I_sc4,200);
e=medfilt2(P,[2 2]);
P=e>0; 

%% Finding remaining pixels in H' and T' after removing V1 from them. i.e.,
%%H1=P1, T1=P2, C=pixel locations in H1 and T1.
P1=(Kn2-P)>0;
P2=(I_sc4-P)>0;
PU=(P1|P2).*g0;
[pixl_r,pixl_c]=find(PU>0);
pix_r=[pixl_r];
pix_c=[pixl_c];
%% ---------------------------------------------------------------------------
%%Next read the manually annotated image for image accuracy computation in
%%test model
actpath = 'Data\STARE\ah\';
act=imread([actpath evalfile '.ah.jpg']);
act=im2double(act).*g;
%% Features extraction to classify the non-major vessel pixels
m1=[5];
for i=1:length(pix_r)
    tst_data.X(1,tst_ind)=HH0(pix_r(i),pix_c(i));
    f_id=2;
for mi=1:length(m1)
    m=m1(mi);
    x=pix_r(i);
    y=pix_c(i);
    clear pix;
    if(x>m-2 && x<500-m+2)
        sub_im_x=x-m+2:x+m-2;
    elseif(x<m-1)
        sub_im_x=x:x+m-2;
    else
        sub_im_x=x-m+2:x;
    end
    if(y>1 && y<500)
        sub_im_y=y-1:y+1;
    elseif(y==1)
        sub_im_y=y:y+1;
    else
        sub_im_y=y-1:y;
    end
        
       pix=[];
        for k1=1:length(sub_im_x)
            for k2=1:length(sub_im_y)
                pix=[pix HH0(sub_im_x(k1),sub_im_y(k2))];
            end
        end
        tst_data.X(f_id,tst_ind)=mean(pix);
        tst_data.X(f_id+1,tst_ind)=sqrt(var(pix));
        tst_data.X(f_id+2,tst_ind)=max(pix);
        tst_data.X(f_id+3,tst_ind)=min(pix);
        tst_data.X(f_id+4,tst_ind)=length(find(pix>HH0(x,y)))/(length(pix));
        f_id=f_id+5;
end

tst_ind=tst_ind+1;
end
%% Pixel classification using GMM classifier----------------------------------------------------
 OBJ=gmdistribution.fit(trn_data.X',2);
post=posterior(OBJ,tst_data.X');
thresh=0.98;
   ind=find(post(:,1)>=thresh);
   ypred=zeros(1,length(post));
   ypred(ind)=1;
 
%% The segmented vasculature includes the major vessels first, Rec=P.
Rec=P;
%% Next, all the pixels that were classified by the GMM classifier are added to the vasculature image Rec.
for i=1:length(pix_r)
    if(ypred(i)==1)
        Rec(pix_r(i),pix_c(i))=1;
    end
end
%% Finally post-processing
Rec=medfilt2(Rec,[2 2]);
Rec=bwareaopen(Rec,40);
%% Timing analysis
timing(j)=toc(tstart);
hl=imshow(Rec);
%% Save the finally segmented image
 path='Data\STARE\segmented vessels\';
  save2=[path evalfile '.jpg'];
  imwrite(Rec,save2);
  %% Compute the accuracy of vessel segmentation in test mode
[sen(j), spec(j),PPV(j),NPV(j),Acc(j)]=sen_spec_image_Marin(act.*g0,Rec.*g0,u,v,pixg);
end
