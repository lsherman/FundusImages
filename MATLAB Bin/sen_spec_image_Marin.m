function[sen, spec,PPV,NPV,Acc]=sen_spec_image_Marin(act,expo,u,v,pix)
TP=0;TN=0;FP=0;FN=0;Acc=0;
%act=imresize(act,[650 550],'nearest');
%expo=imresize(expo,[650 550],'nearest');
%g=imresize(g,[650 550],'nearest');


%% Find the performance within the mask 'g' region only
r=size(pix,1);
for i=1:r
    x=pix(i,2);
    y=pix(i,1);
    if(act(x,y)>0 & expo(x,y)>0)
        TP=TP+1;
    elseif(act(x,y)>0 & expo(x,y)==0)
        FN=FN+1;
    elseif(act(x,y)==0 & expo(x,y)==0)
        TN=TN+1;
    else
        FP=FP+1;
    end
end

Acc=(TP+TN)/(TP+TN+FP+FN);
if (TP>0 || FN>0)
    sen=TP/(TP+FN);
else
    sen=1;
end
if (TN>0 || FP>0)
    spec=(TN/(TN+FP));
else
    spec=1;
end
if (TP>0 || FP>0)
    PPV=TP/(TP+FP);
else
    PPV=1;
end
if (TN>0 || FN>0)
    NPV=TN/(TN+FN);
else
    NPV=1;
end

end