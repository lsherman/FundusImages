clear all;
close all;
clc;
%% This is the 1st code to be run for training GMM classifier
%% First set all the paths to the images %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DRIVE_configure;
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

imfiles      =textread(roi.evalfiles,'%s','delimiter','\n');
numfiles    = size(imfiles,1);
testfiles    =textread(roi.testfiles,'%s','delimiter','\n');
numtest     = size(testfiles,1);
tstactfiles     = textread(roi.testactfile,'%s','delimiter','\n');
actfiles     = textread(roi.actfile,'%s','delimiter','\n');
timing=zeros(1,numtest);

%% Set the backgrund masks%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
g3=imread('Data\DRIVE_second.jpg');
     g0=im2double(g3(:,:,2))>0.9;
     g0=imresize(g0,[500 500],'nearest');
     
     g2=imread('Data\DRIVE_mask_1.jpg');
     g=im2double(g2(:,:,2))>0.9;
     g=imresize(g,[500 500],'nearest');
     trn_ind=1;
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Next begin to create the training data for the GMM classifier using 20 trainig images frm DRIVE Train data set
 for j=1:2%numfiles

 fprintf('Training image number= %d\r',j);
     evalfile = deblank(imfiles{j});
     tstart=tic;
     I = imread([roi.evalfilepath{1} evalfile(:,1:end) '.tif']);
     K=(I(:,:,2));    
     
%% Contrast adjustment and resizing images      
R=im2double(imresize(I(:,:,1),[500 500],'nearest'));
[u,v]=size(g);
H=im2double(K);
H1=im2double(K);
HH0=imadjust(imresize(H,[u v],'nearest'));
%% High pass filter image extraction (H=I_sc3)
w=fspecial('average',[10 10]);
I_bg=imfilter(HH0,w,'replicate');
I_sc=HH0-I_bg;
[r,c]=find(I_sc>=0);
for i=1:size(r,1)
    I_sc(r(i),c(i))=0;
end
I_sc1=abs(I_sc);
I_neg=(1-HH0).*g;
I_sc3=imadjust(I_sc1.*g);
I_sc3=enhancer_new(I_sc3,1);


%% Extraction of morohologically tophat recosntructed image (T=Kn1)

U=enhancer(imadjust(I_neg),2);
for h=1:12
    se=strel('line',9,(h-1)*15);
    im_top=imtophat(U,se);
    images(h,:)=reshape(im_top,1,u*v);
end
max_top=max(images,[],1);
Kn1=reshape(max_top,u,v);
ex_im=Kn1;
Kn1=imadjust(enhancer(Kn1,1));

%% Extraction of two binary images, (H'=Kn2, T'=I_sc4)
Kn2=Kn1>0.2;
Kn2=bwareaopen(Kn2,30);
I_sc4=I_sc3>0.2;
I_sc4=bwareaopen(I_sc4,30);

%% Finding the intersecting regions in the two binary images as the major vessels (P=V1=H' intersect T')
P=Kn2.*I_sc4;
e=medfilt2(P,[2 2]);
P=e>0; 

%% Finding remaining pixels in H' and T' after removing V1 from them. i.e.,
%%H1=P1, T1=P2, C=pixel locations in H1 and T1.
P1=(Kn2-P)>0;
P2=(I_sc4-P)>0;
[pix1_r,pix1_c]=find(P1>0);
[pixl_r,pixl_c]=find(P2>0);
pix_r=[pix1_r;pixl_r];
pix_c=[pix1_c;pixl_c];
%%-------------------------------------------------------------------
%%Next read the manually annotated image for image accuracy computation in
%%test model
actualfile = deblank(actfiles{j});
act=imread([roi.actpath{1} actualfile '.gif']);
act=imresize(act,[u v],'nearest');
act=im2double(act).*g;

%% Features extraction to create training data. trn_data.X is [6xn], corresponding to 6 features extracted per pixel, and n is the number of pixels
%%trn_data.y=[0,1]. If pixel is a vessel, trn_data.y=1, else trn_data.y=0.
m1=[5];
for i=1:length(pix_r)
    trn_data.X(1,trn_ind)=HH0(pix_r(i),pix_c(i));
    %trn_data.X(3,trn_ind)=HH0(pix_r(i),pix_c(i));
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
        trn_data.X(f_id,trn_ind)=mean(pix);
        trn_data.X(f_id+1,trn_ind)=sqrt(var(pix));
        trn_data.X(f_id+2,trn_ind)=max(pix);
        trn_data.X(f_id+3,trn_ind)=min(pix);
        trn_data.X(f_id+4,trn_ind)=length(find(pix>HH0(x,y)))/(length(pix));
        f_id=f_id+5;
end
    if(act(pix_r(i),pix_c(i))>0)
        trn_data.y(trn_ind)=1;
    else
        trn_data.y(trn_ind)=0;
    end
    trn_ind=trn_ind+1;
end

end
%% Next Test on DRIVE
%load DRIVE_new;
DRIVE_trn_n=trn_data;
save DRIVE_trn_n;
