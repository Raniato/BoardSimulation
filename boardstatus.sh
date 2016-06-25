#!/bin/bash
existing_dir=0
running=0
for k in $(ls $1)
do
	if [ -d $1$k ]
	then directory=$k
		myPID=$(cat  $1$directory/'spid.txt')
		myPID=$(($myPID+0))
		existing_dir=$((existing_dir + 1)) 
		if ps -p $myPID > /dev/null
		then
			running=$((running+1))
		fi
	fi 
done
echo "$running Boards Active:"
for k in $(ls $1)
do
	if [ -d $1$k ]
	then directory=$k
		myPID=$(cat  $1$directory/'spid.txt')
		myPID=$(($myPID+0))
		if ps -p $myPID > /dev/null
		then
			echo "$1$directory"
		fi
	fi
done
echo "$(( existing_dir-running )) inactive Boards (on disk but no server running):"
for k in $(ls $1)
do
	if [ -d $1$k ]
	then directory=$k
		myPID=$(cat  $1$directory/'spid.txt')
		myPID=$(($myPID+0))
		if ! ps -p $myPID > /dev/null
		then
			echo "$1$directory"
		fi
	fi
done
