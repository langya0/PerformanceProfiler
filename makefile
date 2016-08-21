perfomance:main.cpp
	@g++ main.cpp -w -lpthread -o perfomance

.PHONY:clean
clean:
	@rm perfomance