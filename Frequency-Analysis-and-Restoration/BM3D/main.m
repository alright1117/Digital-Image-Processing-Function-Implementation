%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The implementation of the BM3D paper (Image Denoising by Sparse 3-D 
% Transform-Domain Collaborative Filtering)
% For more information, you can see the following two papers:
% https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=4271520
% http://www.ipol.im/pub/art/2012/l-bm3d/article.pdf
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;clear;
sigma=0.1225; window_size= 8; search_width= 19; 
l2= 0; selection_number = 8; l3= 2.7;

org_img = imread('Noisy.png');

imwrite(org_img,'results/input_image.jpg');

noisy_img = padarray(org_img,[search_width search_width], ...
        'symmetric','both');

basic_result = first_step(noisy_img, sigma, ...
        window_size, search_width, l2, l3, selection_number);

basic_padded = padarray(basic_result, ...
        [search_width search_width],'symmetric','both');
    
final_result = second_step(noisy_img, basic_padded, ...
        sigma, window_size, search_width, l2, selection_number);

noisy_img = noisy_img(search_width+1:end-search_width, ...
    search_width+1:end-search_width,:);

imwrite(noisy_img,'results/noisy_image.jpg');
imwrite(uint8(basic_result),'results/res_phase1.jpg');
imwrite(uint8(final_result),'results/res_phase2.jpg');