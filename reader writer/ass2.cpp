	#include <iostream>
	#include <vector>
	#include <thread>
	#include <mutex>
	#include <queue>
	#include <condition_variable>
	#include <chrono>
	using namespace std;
	// creating the mutex variables to perform operation on critical section
	mutex read, write, order;
	// this is a conditional variable to hold more than one threads in waiting state
	condition_variable cv;
	//a priority queue for maintaining priority among guests
	priority_queue<int,vector<int>,greater<int>> pq1;//this will store the coupon no. of guests which will do reading in ascending order
	priority_queue<int,vector<int>,greater<int>> pq2;// this will store the coupon no. of guests which will do writing in ascending order
	// a  1000 size array to check if a coupon already assigned or not
	bool random_array_store[1000];
	//variable to count the no of readers at a given time 
	int reading_count = 0;
 	// fucntion to write on the card
	void write_card(int c){
		
       	unique_lock<mutex> ul(order);
        //those write operation whose priority is higher than any other writer and reader will be allowed to write in the card
        // others will wait for there turn 
		cv.wait(ul, [c]{return (pq2.top() == c && pq1.top()>=c);});
 		// we are locking the write lock which is a blocking call means if lock is available then it will access the critical section
        write.lock();
       	cout <<"\n guest \t" << c << " is writing";
       			// we are just making this part take a short time to perform because it takes time to write on the card
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
       	
        write.unlock();
 		// here we will remove the coupon from the priority queue to allow others to perform operations
        pq2.pop();
        // after removing the current coupon we will notify all the guests to check whether its there turn or not
        cv.notify_all();
	}
 	//this function will read from the card
	void read_card(int c)
	{
		unique_lock<mutex> ul(order);
      	cv.wait(ul, [c]{return (pq1.top() == c);});
 
        read.lock();
        reading_count ++;
        if(reading_count == 1) // to avoid busy waiting by other readers if one reader is already reading
                write.lock();
        read.unlock();
 		// once a reader is reading it will remove its coupon no. form the queue and will allow others to chck the critical section
		pq1.pop();
 		cv.notify_all();
        //perform read
       	cout<<"\n guest \t" << c << " is reading";
       	
 		std::this_thread::sleep_for(std::chrono::milliseconds(500));
       	// cout<<"no of reader currently"<<reading_count<<endl;
		read.lock();
        //decrementing the read counter
        reading_count--;
        if(reading_count == 0)// to be done by last reader
            write.unlock();
        read.unlock();
        
        
	}
 	//this is function to assign coupons to the guests
	void assign_coupon(vector<int>&coupon,int n)
 	{
 		for(int i=0; i<n; ++i)
		{
            int temp= rand()%900+100;//since the 3 digit no is in range 100 to 999
            while(random_array_store[temp])
            {
            	temp= rand()%900+100;
			}
			random_array_store[temp]=true;
			coupon[i]=temp;
			// since we have two priority queues so will insert in both the queues 
			pq1.push(temp);
			pq2.push(temp);
		}
 	
	}
	 
	int main(){
		//
        srand((unsigned)time(0));
        //to check if a random no is already taken or not
		for(int i=0;i<1000;i++)
			random_array_store[i]=false;
		int n;
		
       	//taking inputs from user for no of guests
		cout << "No. of guests:\t";
        cin >> n;
 		// we are creating two threads for readig and writing. 
 		// a guest will do both read and write 
        vector<thread> read(n);
        vector<thread> write(n);
        //each guest will have a unique 3 digit id called coupon
		vector<int> coupon(n);
		assign_coupon(coupon,n);
        //
        for(int i=0; i<n; ++i)
        {
        	read[i] = thread(read_card, coupon[i]);
			write[i]= thread(write_card, coupon[i]);
		}
		//this is to join the threads with the main thread so that the main thread will wait for the peer threads
       for(int i=0;i<n;i++)
       {
      		read[i].join();
       		write[i].join();
	   }
 
        return 0;
}
