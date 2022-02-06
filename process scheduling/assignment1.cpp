	#include<iostream>
	#include<fstream>
	#include<vector>
	#include <queue>
	#include <stdio.h>
	#include <map>
	#include <stdlib.h>
	
	//#include<time>
	 
	using namespace std;
	
	class proc{
	        private:
	                int burst;
	                int init_burst;
	                int pid;
	                int atime;
	                int end_time;
	                int waiting_time;
	                
	        public:
	                proc(int pid,int burst,int atime){
	                        this->burst=rand()%8+1;
	                        this->pid=pid;
	                        this->atime=atime;
	                        this->init_burst=burst;
	                        this->end_time=0;
	                }
	                bool isDone(){
	                        return !burst;
	                }
	                void setEndTime(int time){
	                	this->end_time=time;
	            	}
	                void RedByOne(){
	                        this->burst--;
	                }
	                int getid(){
	                	return pid;
					}
					int getBurstTime(){
						return burst;
					}
					int getArrivalTime(){
						return atime;
					}
					int getEndTime(){
						return this->end_time;
					}
					int getWaitingTime(){
						return this->waiting_time;
					}
					void setWaitingTime(){
						this->waiting_time=end_time-atime-init_burst;
					}
					
	        	friend struct compare;
	};
	vector<vector<int> > readInput(string fname){
		ifstream inp;
		inp.open(fname.c_str());
		vector<vector<int> > res;
		while(!inp.eof()){
			vector<int> temp;
			for(int i=0;i<10;i++){
				int num;
				inp>>num;
				temp.push_back(num);
			}
			res.push_back(temp);
		}
		return res;
	}
	struct compare{
	        bool operator()(const proc &a,const proc&b){
	                if(a.burst==b.burst) {
	                        if(a.atime==b.atime){
								return a.pid>b.pid;
							}
							return a.atime>b.atime;
	                }
	                return a.burst>b.burst;
	        }
	};
	int  main(){
		vector<vector<int> > inp=readInput("arrival.txt");//read the file and save arrival times in inp vector
		for(int aind=0;aind<5;aind++){//loop over all arrival times
			cout<<"\n\n**************Starting New Iteration code"<<endl;
			vector<int> res;
			vector<int>arrive(11);
			priority_queue<proc ,vector<proc >,compare> pq;
			map <int ,vector<proc> > aTime;//will map time to processes arriving at that time
			vector<int> inputAtime=inp[aind];
	//		cout<<"printing aTime"<<endl;
	//		for(int i=0;i<10;i++)cout<<" "<<inputAtime[i];
			cout<<endl;
			vector<proc> ord;
			int cnt_rem=0;
			//cout<<"Printing process details\n";
			//cout<<"PID\t\tArrivalTime\t\tBurstTime"<<endl;
			vector<int> endTime(11),waitTime(11),burstTime(11);
			for(int i=1;i<=10;i++){
				//loop over 10 processes, here orders
				vector<int> a(4,0);
				proc temp(i,i%2+1,inputAtime[i-1]);// pid 0, atime 5
				burstTime[i]=temp.getBurstTime();
				aTime[inputAtime[i-1]].push_back(temp);//  
				ord.push_back(temp);
			//	cout<<temp.getid()<<"\t\t\t"<<temp.getArrivalTime()<<"\t\t\t"<<temp.getBurstTime()<<endl;
				arrive[i]=temp.getArrivalTime();
				cnt_rem++;
			}
			
	//		cout<<"cnt_rem "<< cnt_rem<<endl;
			if(aTime[0].size()>0) {
					//add all the processes/orders into heap
					vector<proc> temp=aTime[0];
					for(int i=0;i<temp.size();i++) pq.push(temp[i]);
			}
			int time=0;
			int maxrun=100;
			while(cnt_rem and maxrun){
				maxrun--;
	//			cout<<"time is "<<time<<"pq size"<<pq.size()<<" aTime[time]:"<<aTime[time].size()<<endl;
			
				if(pq.size()==0)res.push_back(-1);
				else{
					proc temp=pq.top();
					pq.pop();
					temp.RedByOne();
					res.push_back(temp.getid());
					if(temp.isDone()){
						cnt_rem--;
						temp.setEndTime(time+1);
						temp.setWaitingTime();
						waitTime[temp.getid()]=temp.getWaitingTime();
						endTime[temp.getid()]=temp.getEndTime();
						
						
					}
					else{
						pq.push(temp);
					}
				}
				time++;
				//loop to add the elements of arrival time i to heap
				if(aTime[time].size()>0) {
					vector<proc> temp=aTime[time];
					for(int i=0;i<temp.size();i++) pq.push(temp[i]);
				}
				
			}
			cout<<"order ID \t"<<"Arrival Time\t"<<"Burst Time\t"<<"finish Time"<<"\t"<<" Waiting Time"<<endl;
			float waittime=0.0;
			for(int i=1;i<11;i++)
			{
				cout<<"  "<<i<<"\t\t "<<arrive[i]<<"\t\t"<<burstTime[i]<<"\t\t "<<endTime[i]<<"\t\t\t"<<waitTime[i]<<endl;
				waittime+=waitTime[i];
			}
			cout<<"\naverage waiting time for all orders is"<<"\t"<<waittime/10<<endl;
			vector<int>newres;
			if(res[0]>=0)
				newres.push_back(res[0]);
			for(int i=1;i<res.size();i++)
			{
				if(res[i]!=res[i-1]&&res[i]>0)
					newres.push_back(res[i]);
			}
			cout<<"\nGantt Chart"<<endl;
			for(int i=0;i<newres.size();i++)
				cout<< " "<<newres[i];
		}
	
	    return 0 ;
	}                                                                                                                                                                                                                                                                                                                                                                                                                    
