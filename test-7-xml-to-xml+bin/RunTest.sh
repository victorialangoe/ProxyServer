# Testfile generator: ./makeTestfiles A X XYZ 12

#!/bin/bash

xterm -ls -e "./proxy 7653" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "./anyReceiver X X 127.0.0.1 7653 30" &
sleep 1
xterm -ls -e "./anyReceiver Y B 127.0.0.1 7653 30" &
sleep 1
xterm -ls -e "./anyReceiver Z B 127.0.0.1 7653 30" &
sleep 5

./xmlSender A 127.0.0.1 7653
