# test file creation ./makeTestfiles A X X 12149
# test file creation ./makeTestfiles B B X 12150

#
# Note: because of the zero sleep values, reordering is normal
#

#!/bin/bash

xterm -ls -e "./proxy 7655" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "./anyReceiver X X 127.0.0.1 7655 20" &
sleep 5

xterm -ls -e "./xmlSender A 127.0.0.1 7655" &

./binSender B 127.0.0.1 7655
