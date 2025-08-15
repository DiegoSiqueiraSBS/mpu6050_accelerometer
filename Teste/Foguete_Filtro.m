clear all
close all
data1 = csvread('sem_tremelique.csv', 1, 0);
ax = 9.81*data1(:,1);
taxa = 0.01;
Ts = taxa;
Fs = 1/taxa;
t = 0:taxa:(length(ax)-1)*taxa;


%%%%%Filtro IIR butterworth ou chebyshev
Fr = 7;
Fr2 = 20;
OmegaC = 2*pi*Fr;% freq angulr
OmegaC2 = 2*pi*Fr2;% freq angulr
w = 0:0.1:10*OmegaC;
N = 3; % Calculo da ordem do filtro
OmegaD = OmegaC/(2*pi*Fs); % freq normalizada
OmegaD2 = OmegaC2/(2*pi*Fs); % freq normalizada
figure
[bd,ad] = butter(N,OmegaD,'high'); %% Definição dos parametros do butterworth analogico
Gsd = tf(bd,ad,Ts);
rlocus(Gsd)
[bd2,ad2] = butter(2*N,OmegaD2,'low');

figure
freqz(bd,ad);
figure
freqz(bd2,ad2);
y1 = filter(bd,ad,ax); %passa alta
y1 = filter(bd2,ad2,y1); % passa baixa
v(1) = 0;
for x = 2:length(y1)
  v(x) = v(x-1) + Ts/2*(y1(x) + y1(x-1));
endfor
p(1) = 0;
for x = 2:length(v)
  p(x) = p(x-1) + Ts*(v(x) + v(x-1))/2;
endfor

v2 = cumtrapz(y1);
p2 = cumtrapz(v);
figure
plot(t,y1,t,ax)
legend("Filtrado","Ruido")
figure
FreW = 0:Fs/length(fft(ax)):Fs-Fs/length(fft(ax));
plot(FreW,mag2db(2*abs(fft(ax))./length(ax)))
xlim([0 Fs/2])

figure
FreW = 0:Fs/length(fft(y1)):Fs-Fs/length(fft(y1));
plot(FreW,mag2db(2*abs(fft(y1))./length(y1)))
xlim([0 Fs/2])

figure
plot(t,p)
