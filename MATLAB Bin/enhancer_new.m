function y=enhancer_new(yin,t)
y=yin.^t;
y=(y-min(y(:)))./(max(y(:))-min(y(:)));
end