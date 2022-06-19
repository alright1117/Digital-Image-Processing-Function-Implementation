clc;
clearvars;

T = imread('im2.ppm'); %read input image
D = imread('disp2.pgm'); %read input depth
scale = 8;

[height,width]=size(D);

view = zeros(height,width,3,'uint8'); %create blank virtual view
hole_map = ones(height,width,'logical'); %create hole map, 1 for none-hole, 0 for hole

%% Modify below code

% LEFT warping
% for y = 1:height
%     for x = width:-1:1
%         warp_location = x + double(D(y,x)/scale);
%         if(warp_location<=width)
%             view(y,warp_location,:) = T(y,x,:);
%             hole_map(y,warp_location) = 0;
%         end
%     end
% end

% RIGHT warping
for y = 1:height
    for x = 1:1:width
        warp_location = x - double(D(y,x)/scale);
        if(0<warp_location)
            view(y,warp_location,:) = T(y,x,:);
            hole_map(y,warp_location) = 0;
        end
    end
end

%% Hole Filling. If you modify below code to enhance image quality, you will get bonus score

after_fill=holefill(view, hole_map);

%% Quality Check
% goal = imread('im0.ppm'); % for LEFT warping
goal = imread('im4.ppm'); % for RIGHT warping

Score_PSNR = psnr(after_fill, goal);
Score_SSIM = ssim(after_fill, goal);

%% Generate Red-Cyan 3D view
% for LEFT warping
% RC_view(:,:,1)=after_fill(:,:,1);
% RC_view(:,:,2)=T(:,:,2);
% RC_view(:,:,3)=T(:,:,3);

% for RIGHT warping
RC_view(:,:,1)=T(:,:,1);
RC_view(:,:,2)=after_fill(:,:,2);
RC_view(:,:,3)=after_fill(:,:,3);

%% image show
figure(1);imshow(T);title('Central Image');
figure(2);imshow(D);title('Depth Map');
figure(3);imshow(after_fill);title('Virtual Image');
figure(4);imshow(goal);title('Ground Truth');
figure(5);imshow(RC_view);title('Red-Cyan 3D view');