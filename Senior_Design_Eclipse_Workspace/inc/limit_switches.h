#ifndef LIMIT_SWITCHES_H_
#define LIMIT_SWITCHES_H_

typedef enum
{
	Top,
	Bottom,
	Claw
}limit_id_t;

class Limit{
public:
	Limit(void);

	void setState(limit_id_t id, bool state);
	bool getState(limit_id_t id);

private:
	limit_id_t switch_id_;
};
#endif
