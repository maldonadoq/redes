#ifndef _WORM_H_
#define _WORM_H_

#include <deque>

class TWorm{
public:
	std::deque<std::pair<int, int> > m_body;
	std::pair<int, int> m_head;

	TWorm();
	~TWorm();

	void Grow(int, int);
	void SetHead(int, int);
	int  RemoveBody();
	void Move(char, unsigned, unsigned);
	bool IsThere(int, int);
	void AddQueue(int);
	int  GetSize();
};

TWorm::TWorm(){

}

void TWorm::Grow(int _x, int _y){
	this->m_body.push_back(std::make_pair(_x,_y));
}

void TWorm::SetHead(int _x, int _y){
	this->m_head = std::make_pair(_x,_y);
	this->m_body.push_back(m_head);
}

int TWorm::RemoveBody(){

	int t = m_body.size()-1;

	this->m_body.clear();
	this->m_body.push_back(m_head);

	return t;
}

void TWorm::Move(char _move, unsigned _r, unsigned _c){
	std::pair<int, int> head;
	head = this->m_body.front();

	// std::cout << "move worm: " << _move << "\n";
	if(!m_body.empty()){
		switch(_move){
			case 'w':
			case 'W':{
				if(head.first == 1)
					head.first = _r-2;
				else
					head.first -= 1;
				this->m_body.push_front(head);
				this->m_body.pop_back();
				break;
			}
			case 's':
			case 'S':{
				if(head.first == _r-2)
					head.first = 1;
				else
					head.first += 1;
				this->m_body.push_front(head);
				this->m_body.pop_back();
				break;
			}
			case 'a':
			case 'A':{
				if(head.second == 1)
					head.second = _c-2;
				else
					head.second -= 1;
				this->m_body.push_front(head);
				this->m_body.pop_back();
				break;
			}
			case 'd':
			case 'D':{
				if(head.second == _c-2)
					head.second = 1;
				else
					head.second += 1;

				this->m_body.push_front(head);
				this->m_body.pop_back();
				break;
			}
			default:
				break;
		}

		m_head = head;
	}	
}

bool TWorm::IsThere(int _r, int _c){
	for(auto pos:m_body){
		if((pos.first == _r) and (pos.second == _c))
			return true;
	}

	return false;
}

void TWorm::AddQueue(int t){
	std::pair<int, int> tmp = m_body.back();

	// std::cout << "AddQueue: " << t << "\n";
	for(int i=0; i<t; i++){
		m_body.push_back(tmp);
	}
}

int TWorm::GetSize(){
	return (int)m_body.size();
}

TWorm::~TWorm(){

}

#endif