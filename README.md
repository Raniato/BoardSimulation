# BoardSimulation
Board simulation using pipes (developed using Linux)

Board.c execv's to a new program which communicates with the first through pipes. 
Absolutely 0 busy waiting (it was forbidden from our prof anyways), using select to wait for incoming data to READ end of pipe.
The board client (boardpost) can send messages to a channel or even texts. While on my machine everything was OK, in some machines in Uni some functions were not exactly working correctly.

execute as:
% ./board <path>
where path is the place on your C drive where a board will be made.
This is resembling a server, so the user handling this server can:
1. createchannel <id> <name>
2. getmessages <id>
3. exit (just exits, server is not shut down)
4. shutdown
% ./boardpost <path>
1. list
show every board in <path>
2. write <id> <message>
write a <message> to the channel <id>.
3. send <id> <file>
send a <file> to the channel <id>.

For any other questions or further help, contact me at rania.spantidi[at]gmail.com :D
(Greek speaking friends, read comments and readme for further info)
