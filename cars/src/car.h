#ifndef _CAR_H_
#define _CAR_H_

class TCar{	
	int m_speed;	
public:
	int  m_x;
	int  m_y;
	char m_avatar;

	TCar(){
		this->m_avatar = ' ';
	}

	void SetSpeed(int _speed){
		this->m_speed = _speed;		
	}

	void SetPosition(int _x, int _y){
		this->m_x = _x;
		this->m_y = _y;
	}

	void SetAvatar(char _av){
		this->m_avatar = _av;
	}

	bool Move(char _move, unsigned _c){
		switch(_move){
			case 'o':
			case 'O':{				
				m_speed++;				
				break;
			}
			case 'l':
			case 'L':{				
				m_speed--;
				break;
			}
			case 'w':
			case 'W':{
				m_x -= 1;
				if(m_x < 1)
					return false;
				break;
			}
			case 's':
			case 'S':{
				m_x += 1;
				if(m_x >= (_c-1))
					return false;
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
				if(m_y >= (_c-2))
					return false;
				break;
			}
			default:
				break;
		}

		return true;
	}

	bool Move(int road_speed, unsigned _r){
		m_x += (road_speed - m_speed);

		if((m_x > 0) and (m_x < (_r - 1)))
			return true;

		return false;
	}

	int GetSpeed(){
		return this->m_speed;
	}

	~TCar(){	};
};

#endif