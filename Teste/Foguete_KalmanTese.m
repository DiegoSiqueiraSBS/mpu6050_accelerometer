clear all;
clc;

pkg load signal;

x0 = [0; 0];  % initial state (location and velocity)
P0 = [1000 0; 0 1000];  % initial uncertainty

global u = [0; 0];  % external motion

% motion model
% x' = 1 * x + t * v
% v' = 0 * x + 1 * v
% apply t = 1
global F = [1 1; 0 1];  % next state function
global H = [1 0];  % measurement function

global Id = eye(2);  % identity matrix

global R = [1];  % measurement uncertainty

function [ret_x, ret_P] = kalman_update (Z, x, P)
  global H;
  global R;
  global Id;

  y = Z - H * x;  % y is a residual
  S = (H * P * H') + R;
  K = P * H' * inv(S);

  ret_x = x + K * y;
  ret_P = (Id - K * H) * P;
endfunction

function [ret_x, ret_P] = kalman_prediction(x, P)
  global F;
  global u;

  ret_x = F * x + u;
  ret_P = F * P * F';
endfunction

function [ret_x, ret_P] = kalman_filter (Z, x, P)
  [x_U, P_U] = kalman_update(Z, x, P);
  [ret_x, ret_P] = kalman_prediction(x_U, P_U);
endfunction

function [x, v] = kalman_evaluate(measurements, x0, P0)
  x = [];
  v = [];

  x_i = x0;
  P_i = P0;
  for Z = measurements
    [x_i, P_i] = kalman_filter([Z], x_i, P_i);
    x(end + 1) = x_i(1);
    v(end +1) = x_i(2);
  endfor
endfunction

data1 = csvread('10cmTest.csv', 1, 0);
ax = 9.81*data1(:,1);

taxa = 0.01;
Ts = taxa;
Fs = 1/taxa;
t = 0:taxa:(length(ax)-1)*taxa;

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
y1 = filter(bd,ad,ax); %passa alta
measurements = ax';
[x, v] = kalman_evaluate(measurements, x0, P0);
plot(t,x,t,measurements)
##figure
##plot(t,v)
v2 = cumtrapz(x);
p2 = cumtrapz(v);
figure
plotyy(t,p2,t,x)
