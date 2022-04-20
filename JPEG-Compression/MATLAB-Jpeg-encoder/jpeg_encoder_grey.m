function jpeg_encoder_grey (inputName, outputName)
    CELL_SIZE = 8; %greater than 4

    gray = imread(inputName);
    fid = fopen(outputName, 'w');
    
    %%% Turn into cells 8x8
    repeat_height = size(gray, 1)/CELL_SIZE;
    repeat_width = size(gray, 2)/CELL_SIZE;
    repeat_height_mat = repmat(CELL_SIZE, [1 repeat_height]);
    repeat_width_mat = repmat(CELL_SIZE, [1 repeat_width]);
    gray_sub_image = mat2cell(gray, repeat_width_mat, repeat_height_mat);
    
    writeHeader(fid, size(gray, 1), size(gray, 2));

    lastDC(1) = 0;

    for i=1:repeat_height
        for j=1:repeat_width
            gray_sub_image{i, j} = dcTransform(gray_sub_image{i, j});
            gray_sub_image{i, j} = quantize(gray_sub_image{i, j}, 'lum');
            lastDC(1) = huffman(fid, gray_sub_image{i, j}, lastDC(1), 1, 1);
        end
    end

    flashBuffer(fid);
    
    writeEOI(fid);
    
    fclose(fid);    
end

function writeHeader(fid, height, width)
    global quantLumMatrix;
    global dc_luminance_nrcodes;
    global dc_luminance_values;
    global ac_luminance_nrcodes;
    global ac_luminance_values;

    global zigZagOrder;
    global bits;
    global values;

    %SOI marker
    soi(1) = hex2dec('FF');
    soi(2) = hex2dec('D8');
    fwrite(fid, soi);

    %JFIF Header
    jfif(1) = hex2dec('FF');
    jfif(2) = hex2dec('E0'); %marker
    jfif(3) = hex2dec('00');
    jfif(4) = hex2dec('10'); %16
    jfif(5) = 'J';
    jfif(6) = 'F';
    jfif(7) = 'I';
    jfif(8) = 'F';
    jfif(9) = hex2dec('00');
    jfif(10) = hex2dec('01'); %vh
    jfif(11) = hex2dec('01'); %vl
    jfif(12) = hex2dec('00');
    jfif(13) = hex2dec('00');
    jfif(14) = hex2dec('01'); %xden
    jfif(15) = hex2dec('00');
    jfif(16) = hex2dec('01'); %yden
    jfif(17) = hex2dec('00');
    jfif(18) = hex2dec('00');
    fwrite(fid, jfif);
    
    %DQT Header
    dqt(1) = hex2dec('FF');
    dqt(2) = hex2dec('DB'); %marker
    dqt(3) = hex2dec('00');
    dqt(4) = hex2dec('43'); 
    dqt(5) = hex2dec('00'); %y_marker

    for i=1:64
        dqt(5+i) = quantLumMatrix(zigZagOrder(i));
    end
    fwrite(fid, dqt);
    
    % Start of Frame Header
    SOF(1) = hex2dec('FF');
    SOF(2) = hex2dec('C0');
    SOF(3) = hex2dec('00');
    SOF(4) = hex2dec('0B');
    SOF(5) = hex2dec('08');
    SOF(6) = bitand(bitshift(height, -8), hex2dec('FF'));
    SOF(7) = bitand(height, hex2dec('FF'));
    SOF(8) = bitand(bitshift(width, -8), hex2dec('FF'));
    SOF(9) = bitand(width, hex2dec('FF'));
    SOF(10) = 1; % 1 component
    SOF(11) = 1; % component id
    SOF(12) = 17; % no subsampling
    SOF(13) = hex2dec('00');

    fwrite(fid, SOF);
    
    % The DHT Header
    DHT4(1) = hex2dec('FF');
    DHT4(2) = hex2dec('C4');
    DHT4(3) = bitand(bitshift(19 + 12, -8), hex2dec('FF'));
    DHT4(4) = bitand(19 + 12, hex2dec('FF'));
    
    for i = 1:16
        DHT4(5+i) = dc_luminance_nrcodes(i+1);
    end

    for i = 1:12
        DHT4(21+i) = dc_luminance_values(i);
    end
    
    DHT4(34) = hex2dec('FF');
    DHT4(35) = hex2dec('C4');
    DHT4(36) = bitand(bitshift(19 + 162, -8), hex2dec('FF'));
    DHT4(37) = bitand(19 + 162, hex2dec('FF'));
    DHT4(38) = hex2dec('10');

    for i = 1:16
        DHT4(38+i) = ac_luminance_nrcodes(i+1);
    end
   
    for i = 1:162
        DHT4(54+i) = ac_luminance_values(i);
    end

    fwrite(fid, DHT4);
    
    % Start of Scan Header
    SOS(1) = hex2dec('FF');
    SOS(2) = hex2dec('DA');
    SOS(3) = bitand(bitshift(8, -8), hex2dec('FF'));
    SOS(4) = bitand(8, hex2dec('FF'));
    SOS(5) = hex2dec('01');
    SOS(6) = hex2dec('01');
    SOS(7) = hex2dec('00');

    SOS(8) = hex2dec('00');
    SOS(9) = hex2dec('3F');
    SOS(10) = hex2dec('00');
    fwrite(fid, SOS);
end

function flashBuffer(fid) 
    global bufferPutBits;
    global bufferPutBuffer;
    global output;
    PutBuffer = bufferPutBuffer;
    PutBits = bufferPutBits;
    while PutBits >= 8 
        c = bitand(bitshift(PutBuffer, -16), hex2dec('FF'));
        fwrite(fid, c);
        
        if c == hex2dec('FF')
            fwrite(fid, 0);
        end
        PutBuffer = bitshift(PutBuffer, 8);
        PutBits = PutBits - 8;
    end
    if PutBits > 0 
        c = bitand(bitshift(PutBuffer, -16), hex2dec('FF'));
        fwrite(fid, c);
    end
    fwrite(fid, output);
end

function writeEOI(fid)
    
    eoi(1) = hex2dec('FF');
    eoi(2) = hex2dec('D9');
    fwrite(fid, eoi);

end

    