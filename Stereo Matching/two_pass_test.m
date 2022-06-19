clear all
clc
tic
test_time = 0;
view = -1; % 1 left view, -1 right view
jump_disparity = 1;
pic= 1;
a=0;
%%%%%%%%%%%%%%%%%image selection%%%%%%%%%%%%%%%%%%
if (pic==1)
    fname ='cones';
    d_size=60;
    scale=4;
elseif (pic==2)
    fname ='teddy'; 
    d_size=60;
    scale=4;
elseif (pic==3)
    fname ='tsukuba';
    d_size=16;
    scale=16;
elseif (pic==4)
    fname ='venus';
    d_size=60;
    scale=4;
elseif (pic==5)
    fname = 'book';
    d_size=80;
    scale=3;
elseif (pic==6)
    fname = 'Art';
    d_size=80;
    scale=3;
elseif (pic==7)
    fname ='conesH';
    d_size=120;
    scale=2;
elseif (pic==8)
    fname ='new';
    d_size=60;
    scale=4;
elseif (pic==11)
    fname ='cones_large';
    d_size=128;
    scale=2;
elseif (pic==12)
    fname ='Dolls';
    d_size=80;
    scale=3;    
elseif (pic==13)
    fname ='tank';
    d_size=60;
    scale=4;    
elseif (pic==14)
    fname ='new';
    d_size=60;
    scale=4;      
elseif (pic==15)
    fname ='Computer';
    d_size=80;
    scale=3;     
elseif (pic==16)
    fname ='Laundry';
    d_size=80;
    scale=3;      
elseif (pic==17)
    fname ='Moebius';
    d_size=80;
    scale=3;       
elseif (pic==18)
    fname ='Reindeer';
    d_size=80;
    scale=3;       
elseif (pic==19)
    fname ='Aloe';
    d_size=80;
    scale=3;      
elseif (pic==20)
    fname ='Baby1';
    d_size=80;
    scale=3;       
elseif (pic==21)
    fname ='test_sun';
    d_size=16;
    scale=16;
elseif (pic==22)
    fname ='shark';
    d_size=60;
    scale=8;
end

if( view == 1 )
    im2 = double(imread(sprintf('%s%s%s%s_imL.png','.\test image\',fname,'\',fname)));
    im6 = double(imread(sprintf('%s%s%s%s_imR.png','.\test image\',fname,'\',fname)));
else
    im6 = double(imread(sprintf('%s%s%s%s_imL.png','.\test image\',fname,'\',fname)));
    im2 = double(imread(sprintf('%s%s%s%s_imR.png','.\test image\',fname,'\',fname)));
end


[height,width,rgb_size] =size(im2);
d_min = 1;

rc = 32;
rc2 = 64;
rmc = 32;

im2Y = zeros( height,width,3 );
im2Y = rgb2ycbcr(im2);
im2(:,:,3) = im2Y(:,:,1);
im6Y = zeros( height,width,3 );
im6Y = rgb2ycbcr(im6);
im6(:,:,3) = im6Y(:,:,1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   modified mini census transform  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

census_L = zeros( height,width,27 );
census_R = zeros( height,width,27 );
census_aL = zeros( height,width,27 );
census_naL = zeros( height,width,27 );
census_aR = zeros( height,width,27 );
census_naR = zeros( height,width,27 );
r = zeros(height*width,1);

for y = 3: 1: height-2
    for x = 3: 1: width-2
        for i = 1: 1: 3
            census_aL(y,x,i)=(im2(y,x,i)+im2(y-2,x,i)+im2(y-1,x,i)+im2(y+1,x,i)+im2(y+2,x,i)+im2(y,x-2,i)+im2(y,x+2,i)+im2(y-2,x-2,i)+im2(y-2,x+2,i)+im2(y+2,x-2,i)+im2(y+2,x+2,i))/11;
            census_naL(y,x,i) = (census_aL(y,x,i)*11-im2(y,x,i))/10;
            census_aR(y,x,i)=(im6(y,x,i)+im6(y-2,x,i)+im6(y-1,x,i)+im6(y+1,x,i)+im6(y+2,x,i)+im6(y,x-2,i)+im6(y,x+2,i)+im6(y-2,x-2,i)+im6(y-2,x+2,i)+im6(y+2,x-2,i)+im6(y+2,x+2,i))/11;
            census_naR(y,x,i) = (census_aR(y,x,i)*11-im6(y,x,i))/10;
        end
    end
end
Lmax=im2(1,1,1);
for y = 2: 1: height-1
    for x = 2: 1: width-1
        for i = 1: 1: 3
            if(im2(y,x,i)>Lmax)
            Lmax=im2(y,x,i);
            end
        end
    end
end
Lmin=im2(1,1,1);
for y = 2: 1: height-1
    for x = 2: 1: width-1
        for i = 1: 1: 3
            if(im2(y,x,i)<Lmin)
            Lmin=im2(y,x,i);
            end
        end
    end
end

delta=5;

census_naL(y,x,i)=im2(y,x,i);
census_aL(y,x,i)=im2(y,x,i);
census_naR(y,x,i)=im6(y,x,i);
census_aR(y,x,i)=im6(y,x,i);

threash=im2(y,x,i)/delta;  

for y = 2: 1: height-1
    for x = 2: 1: width-1
        for i = 1: 1: 3
            for xi = -1: 1: 1            
                for yi = -1: 1: 1
                   if(im2(y+yi,x+xi,i) > im2(y,x,i) && im2(y+yi,x+xi,i) < im2(y,x,i)+threash)
                        census_L(y,x,i+a*3) = 110;
                   elseif(im2(y+yi,x+xi,i) <= im2(y,x,i)-threash)
                        census_L(y,x,i+a*3) = 0;
                   elseif(im2(y+yi,x+xi,i) > im2(y,x,i)-threash && im2(y+yi,x+xi,i) <= im2(y,x,i))
                        census_L(y,x,i+a*3) = 100;
                   elseif(im2(y+yi,x+xi,i) >= im2(y,x,i)+threash)
                        census_L(y,x,i+a*3) = 111;
                   end
                   
                   if(a<=7)
                        a=a+1;
                   else
                        a=0;
                   end
                  
                end
            end
        end
    end
end

threash=im6(y,x,i)/delta;

for y = 2: 1: height-1
    for x = 2: 1: width-1
        for i = 1: 1: 3
             for xi = -1: 1: 1            
                for yi = -1: 1: 1
                   if(im6(y+yi,x+xi,i) > im6(y,x,i) && im6(y+yi,x+xi,i) < im6(y,x,i)+threash)
                        census_R(y,x,i+a*3) = 110;
                   elseif(im6(y+yi,x+xi,i) <= im6(y,x,i)-threash)
                        census_R(y,x,i+a*3) = 0;
                   elseif(im6(y+yi,x+xi,i) > im6(y,x,i)-threash && im6(y+yi,x+xi,i) <= im6(y,x,i))
                        census_R(y,x,i+a*3) = 100;
                   elseif(im6(y+yi,x+xi,i) >= im6(y,x,i)+threash)
                        census_R(y,x,i+a*3) = 111;
                   end
                   
                   if(a<=7)
                        a=a+1;
                   else
                        a=0;
                   end
                  
                end
            end
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% initial cost %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

initial_cost = zeros(height,width,d_size);

for d = d_min :jump_disparity:d_size
    for y = 1: 1: height
        for x = 1: 1: width
          tqx = x - d*view;
          if( tqx >= 1 && tqx<= width)
              census_cost = 0;
              for k = 1: 1: 27
                  if(abs(census_L(y,x,k)-census_R(y,tqx,k)==100)||abs(census_L(y,x,k)-census_R(y,tqx,k)==10)||abs(census_L(y,x,k)-census_R(y,tqx,k)==1))
                      census_cost = census_cost +1;
                  elseif(abs(census_L(y,x,k)-census_R(y,tqx,k)==110)||abs(census_L(y,x,k)-census_R(y,tqx,k)==11))
                      census_cost = census_cost + 2;
                  elseif(abs(census_L(y,x,k)-census_R(y,tqx,k)==111))
                      census_cost = census_cost + 3;
                  end             
              end
              initial_cost(y,x,d) = census_cost;
          else
              initial_cost(y,x,d) = rc+30;
          end
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% weight left image %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

weight_v = double(ones(height-1, width));
weight_h = double(ones(height, width-1));

for y = 1: 1:height
    for x = 1: 1:width
        if( x+1< width)
            sum_abs_diff = (abs( im2(y,x,1) - im2(y,x+1,1) ) + abs( im2(y,x,2) - im2(y,x+1,2) ) + abs( im2(y,x,3) - im2(y,x+1,3) ));           
            weight_h (y,x) = exp(-sum_abs_diff/rc2);
        end
        if( y+1 < height )
            sum_abs_diff = (abs( im2(y,x,1) - im2(y+1,x,1) ) + abs( im2(y,x,2) - im2(y+1,x,2) ) + abs( im2(y,x,3) - im2(y+1,x,3) )); 
            weight_v (y,x) = exp(-sum_abs_diff/rc2);
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% vertical aggregation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

v_agg_cost_U = zeros(height,width,d_size);

for y = 1: 1: height
    for x = 1: 1: width
        for d = d_min :jump_disparity:d_size
            if( y>1 && y <=height  )
                v_agg_cost_U( y,x,d ) = (initial_cost(y,x,d) + weight_v (y-1,x)*v_agg_cost_U( y-1,x,d ));
            else
                v_agg_cost_U( y,x,d ) = initial_cost(y,x,d);
            end
        end
    end
end

v_agg_cost_D = zeros(height,width,d_size);

for d = d_min :jump_disparity:d_size
    for y = height: -1: 1
        for x = 1: 1: width
            if(y>=1 && y <height-1)
                v_agg_cost_D( y,x,d ) = initial_cost(y,x,d) + weight_v (y+1,x)*v_agg_cost_D( y+1,x,d );
            else
                v_agg_cost_D( y,x,d ) = initial_cost(y,x,d);
            end
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% horizontal aggregation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

h_agg_cost_L = zeros(height,width,d_size);

for d = d_min :jump_disparity: d_size
    for y = 1: 1: height
        for x = 1: 1: width
            if( x > 1 && x<width && y>1 )
                h_agg_cost_L( y,x,d ) = (v_agg_cost_U( y,x,d ) + v_agg_cost_D( y,x,d ) + weight_h (y,x-1)*h_agg_cost_L( y,x-1,d ));
            else
                h_agg_cost_L( y,x,d ) = v_agg_cost_U( y,x,d ) + v_agg_cost_D( y,x,d );
            end
        end
    end
end

h_agg_cost_R = zeros(height,width,d_size);

for d = d_min :jump_disparity:d_size
    for y = 1: 1: height
        for x = width: -1: 1
            if( x > 0 && x<width-1 && y>1 )
                h_agg_cost_R( y,x,d ) = (v_agg_cost_U( y,x,d ) + v_agg_cost_D( y,x,d ) + weight_h (y,x+1)*h_agg_cost_R( y,x+1,d ));
            else
                h_agg_cost_R( y,x,d ) = v_agg_cost_U( y,x,d ) + v_agg_cost_D( y,x,d );
            end
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   WTA   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

disparity_out = zeros(height,width);

for y = 1: 1: height-1
    for x = 1: 1: width-1
        dis_min = 50000000000000000000000;
        
        for d = d_min : jump_disparity : d_size
            if( (h_agg_cost_L(y,x,d) + h_agg_cost_R(y,x,d) )  < dis_min)   
                dis_min = ( h_agg_cost_L(y,x,d) + h_agg_cost_R(y,x,d));  
                disparity_out(y,x) = d;
            end
       end
   end
end


if( view ==1 )
    outdepthmap =uint8(disparity_out*scale);
    filename=sprintf('%s%s_disparity_thesis_mmc_p10o_L.png','.\result\',fname);
    imwrite(outdepthmap,filename);
else
    outdepthmap =uint8(disparity_out*scale);
    filename=sprintf('%s%s_disparity_thesis_mmc_p10o_R.png','.\result\',fname);
    imwrite(outdepthmap,filename);
end


test_time = toc - test_time
test_time = toc;

 