function P = Vasc_detection_rework_DRIVE_classify_DRIVE_trn(flname)

%% :TODO: :NOTE: MATLAB Runtime stuff apparently has threading issues,
%%               we will need to work those out if we continue using MATLAB

close all;
clc;
%% This is the 2nd code to be run for test images
%% First set all the paths to the images %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DRIVE_configure;

%% :TODO: This can be done once in preprocessing for the server
testfiles = textscan(roi.testfiles,'%s','delimiter','\n');

%% :TODO: This can be done once in preprocessing for the server
%% Load the background mask used for post processing
maskImg=imread('Data\DRIVE_second.jpg');
mask=im2double(maskImg(:,:,2))>0.9;
mask=imresize(mask,[500 500],'nearest');

%% :TODO: This can be done once in preprocessing for the server
g2=imread('Data\DRIVE_mask_1.jpg');
g=im2double(g2(:,:,2))>0.9;
g=imresize(g,[500 500],'nearest');

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Next begin to test each image for vessel segmentation using the DRIVE train set of images

%% :TODO: This can be done once in preprocessing for the server
load DRIVE_trn_n %This is the training data to be loaded

tst_ind=1;
fprintf('Processing Image...');
evalfile = deblank(testfiles{1});
I = imread(flname);
K=(I(:,:,2));

tstart=tic;

%% Contrast adjustment and resizing images
%% :TODO: Check if this is correct.
%%        We are scaling the input image to the dimensions of the particular MASK?
[u,v]=size(g);
H=im2double(K);
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
I_sc3=imadjust(I_sc1.*g);
I_sc3=enhancer_new(I_sc3,1);

%% Extraction of morphologically tophat reconstructed image (T=Kn1)
I_neg=(1-HH0).*g;
U=enhancer(imadjust(I_neg),2);

images = zeros(12,u*v);
for h=1:12
    se=strel('line',9,(h-1)*15);
    %se=strel('disk',(h-1)*5);
    im_top=imtophat(U,se);
    images(h,:)=reshape(im_top,1,u*v);
end
max_top=max(images,[],1);
Kn1=reshape(max_top,u,v);
Kn1=imadjust(enhancer(Kn1,1));

%% Extraction of two binary images, (H'=I_sc4, T'=Kn2)
Kn2=Kn1>0.2;
Kn2=bwareaopen(Kn2,30);
I_sc4=I_sc3>0.2;
I_sc4=bwareaopen(I_sc4,30);

%% Find the intersecting regions of the two binary images (P=V1=H' intersect T')
P=Kn2.*I_sc4;
e=medfilt2(P,[2 2]);
P=e>0;

%% Extract the non-intersecting components of H' and T'
%%H1=P1, T1=P2, C=pixel locations in H1 and T1.
P1=(Kn2-P)>0;
P2=(I_sc4-P)>0;
[pix1_r,pix1_c]=find(P1>0);
[pixl_r,pixl_c]=find(P2>0);
pix_r=[pix1_r;pixl_r];
pix_c=[pix1_c;pixl_c];

%% Features extraction to classify the non-major vessel pixels
m1=5;
for i=1:round(length(pix_r))
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
            sub_im_y=y-m+2:y+m-2;
        elseif(y==1)
            sub_im_y=y:y+m-2;
        else
            sub_im_y=y-m+2:y;
        end

        l1 = length(sub_im_x);
        l2 = length(sub_im_y);
        pix=zeros(1,l1*l2);
        index = 0;
        for k1=1:l1
            for k2=1:l2
                index = index + 1;
                pix(1,index)=HH0(sub_im_x(k1),sub_im_y(k2));
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
OBJ    = gmdistribution.fit(trn_data.X',2);
post   = posterior(OBJ,tst_data.X');
thresh = 0.9;
ypred = zeros(1,length(post));
ypred(post(:,1)>=thresh)=1;

%% All pixels classified by the GMM classifier are added to the vasculature image Rec.
for i=1:round(length(pix_r))
    if(ypred(i)==1)
        P(pix_r(i),pix_c(i))=1;
    end
end

%% Perform post-processing / Remove small neighborhoods
P=bwareaopen(P,30).*mask;

%% Timing analysis
fprintf('Processing time: %d',toc(tstart));

%% Save the finally segmented image
imwrite(P, [evalfile '.png'], 'png', 'Transparency', [0]);
