function y=enhancer(yin,t)
y=yin.*10^t;
y=y.^t;
y=(y-min(y(:)))./(max(y(:))-min(y(:)));
end