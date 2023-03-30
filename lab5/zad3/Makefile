programs:
	mkfifo mainfifo
	gcc consumer.c -o consumer
	gcc manufacturer.c -o manufacturer

test:
	./consumer ./mainfifo ./destination.txt 5
	./manufacturer ./mainfifo ./files/source1.txt 1 5
	./manufacturer ./mainfifo ./files/source2.txt 2 4
	./manufacturer ./mainfifo ./files/source3.txt 4 3
	./manufacturer ./mainfifo ./files/source4.txt 8 6

clear:
	rm consumer
	rm manufacturer
	rm mainfifo
