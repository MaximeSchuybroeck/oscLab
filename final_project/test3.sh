make all
port=5678
clients=3
echo -e "starting gateway "
#valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes
./sensor_gateway $port $clients &
sleep 3
echo -e 'starting 3 sensor nodes'
./sensor_node 15 1 127.0.0.1 $port &
sleep 2
./sensor_node 21 3 127.0.0.1 $port &
sleep 2
./sensor_node 37 2 127.0.0.1 $port &
sleep 11
killall sensor_node
sleep 30
killall sensor_gateway
