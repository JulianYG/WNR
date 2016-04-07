clear all;
close all;
    delete(instrfindall);
figure;
system('@echo off');
system('cd C:\Users\Tingkai\Desktop\heatshrink');

    s = serial('COM7');
    s.BaudRate = 38400;
    s.FlowControl = 'hardware';
    %size = 20;
    %s.InputBufferSize = size;
    fopen(s);
 file_ctr= 1;
while(1)    
    %while ((s.BytesAvailable)~=size)
    %end
    %out = fread(s,s.BytesAvailable,'uint8');
    out = fread(s,20,'uint8');
    out = out';% transpose to make it a row vector
    data = out;
    out_char = char(out(1:15));
    TK_true = strcmp(out_char,'TKENDTKENDTKEND');
    
while ~TK_true
    out = fread(s,20,'uint8');
    out = out';
    out_char = char(out);
    TK_true = strcmp(out_char,'AAAAAAAAAAAAAAAAAAAA');
    data = [data out];
end
    fprintf('finished collection\n');
    data(end-20:end) = []; % remove delimiter
    filename_before = sprintf('data%d.csv',file_ctr);
    csvwrite(filename_before,data); 
    
    filename_after = sprintf('data%d_comp.csv',file_ctr);

heatshrink_cmd = sprintf('heatshrink -e -w 8 -l 4 %s >>%s',filename_before,filename_after);
disp(heatshrink_cmd)
system(heatshrink_cmd); % perform decompression
    fprintf('finished compression');
    filename_final = sprintf('data%d_decomp.csv',file_ctr);

heatshrink_cmd = sprintf('heatshrink -d -w 8 -l 4 %s >>%s',filename_after,filename_final);
    system(heatshrink_cmd); % perform decompression

intan_input_decomp = csvread(filename_final);
plot(intan_input_decomp);pause(1);
clear data out 
file_ctr = file_ctr+1;

end



    
    