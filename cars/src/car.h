#ifndef _CAR_H_
#define _CAR_H_

class TCar{	
	float m_speed;	
public:
	int  m_x;
	int  m_y;
	char m_avatar;

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

	bool Brake(int _rsize){
		if(m_x < _rsize-3){
			m_x += 1;
			return true;
		}
		return false;
	}

	bool Move(char _move, unsigned _r, unsigned _c){
		switch(_move){
			case 'w':
			case 'W':{				
				m_x -= 3;
				if(m_x < 0){
					m_x = 0;
					return false;
				}
				break;
			}
			case 'a':
			case 'A':{
				m_y -= 1;
				if(m_y < 2)
					return false;				
				break;
			}
			case 'd':
			case 'D':{
				m_y += 1;
				if(m_y > (_c-3))
					return false;
				break;
			}
			default:
				break;
		}

		return true;
	}

	~TCar(){	};
};

#endif