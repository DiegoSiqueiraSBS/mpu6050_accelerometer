clear all
close all

data = csvread('anguloss.csv');

Accelerometer = data(:, 1:3);
Gyroscope = data(:, 4:6);

taxa = 0.01;

time = 0:taxa:(length(data)-1)*taxa;

addpath('quaternion_library');

figure('Name', 'Sensor Data');
axis(1) = subplot(2,1,1);
hold on;
plot(time, Gyroscope(:,1), 'r');
plot(time, Gyroscope(:,2), 'g');
plot(time, Gyroscope(:,3), 'b');
legend('X', 'Y', 'Z');
xlabel('Time (s)');
ylabel('Angular rate (deg/s)');
title('Gyroscope');
hold off;
axis(2) = subplot(2,1,2);
hold on;
plot(time, Accelerometer(:,1), 'r');
plot(time, Accelerometer(:,2), 'g');
plot(time, Accelerometer(:,3), 'b');
legend('X', 'Y', 'Z');
xlabel('Time (s)');
ylabel('Acceleration (g)');
title('Accelerometer');
hold off;
print SensorData.pdf;
linkaxes(axis, 'x');

AHRS = MadgwickAHRS('SamplePeriod', 1/100, 'Beta', 0.55);

quaternion = zeros(length(time), 4);
for t = 1:length(time)
    AHRS.UpdateIMU(Gyroscope(t,:) * (pi/180), Accelerometer(t,:));
    quaternion(t, :) = AHRS.Quaternion;
end

euler = quatern2euler(quaternConj(quaternion)) * (180/pi);

figure('Name', 'Euler Angles');
hold on;
plot(time, euler(:,1), 'r');
plot(time, euler(:,2), 'g');
plot(time, euler(:,3), 'b');
title('Euler angles');
xlabel('Time (s)');
ylabel('Angle (deg)');
legend('\phi', '\theta', '\psi');
hold off;
print Angulos.pdf;
