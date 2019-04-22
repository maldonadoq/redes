#ifndef _CAR_H_
#define _CAR_H_

class TCar{
	int m_x;
	int m_y;
	float m_speed;	
public:
	char  m_avatar;

	TCar(){
		this->m_avatar = ' ';
	}

	void SetSpeed(float _speed){
		this->m_speed = _speed;		
	}

	void SetPosition(int _x, int _y){
		this->m_x = _x;
		this->m_y = _y;
	}

	void SetAvatar(char _av){
		this->m_avatar = _av;
	}

	void Accelerate(float _a){
		this->m_speed += _a;
	}

	~TCar(){	};
};

#endif