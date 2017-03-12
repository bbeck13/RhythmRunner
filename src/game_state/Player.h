//	Joseph	Arhar



#ifndef	PLAYER_H_

#define	PLAYER_H_



#include	"GameObject.h"

#include	"PhysicalObject.h"



#define	PLAYER_MESH	"models/body_of_bike.obj"

#define	WHEEL_MESH	"models/note.obj"



#define	WHEEL_SCALE	0.4

#define	WHEEL_ROTATION_PER_SECOND	12.0

#define	WHEEL_ROTATION_PER_TICK	(WHEEL_ROTATION_PER_SECOND	*	SECONDS_PER_TICK)



class	Player	:	public	GameObject	{

	public:

		//	Player	animation	stuff

		static	const	int	ANIMATION_ENDGAME_BIT	=	1	<<	5;

		static	const	int	ANIMATION_WHEELSPIN_BIT	=	1	<<	6;

		static	const	int	ANIMATION_WHEELSPIN_SLOW_BIT	=	1	<<	7;

		static	const	int	ANIMATION_AERIAL_TILT_BIT	=	1	<<	8;

		enum	Animation	{

				GROUNDED	=	1	|	ANIMATION_WHEELSPIN_BIT,

				JUMPING	=	2	|	ANIMATION_WHEELSPIN_SLOW_BIT	|	ANIMATION_AERIAL_TILT_BIT,

				SUCCESS	=	3	|	ANIMATION_ENDGAME_BIT	|	ANIMATION_WHEELSPIN_SLOW_BIT,

				FAILURE	=	4	|	ANIMATION_ENDGAME_BIT	|	ANIMATION_WHEELSPIN_SLOW_BIT,

		};

		static	std::string	AnimationToString(Animation	animation)	{

				switch	(animation)	{

						case	GROUNDED:

								return	"GROUNDED";

						case	JUMPING:

								return	"JUMPING";

						case	SUCCESS:

								return	"SUCCESS";

						case	FAILURE:

								return	"FAILURE";

						default:

								return	"unknown	animation";

				}

		}



		enum	DuckDir	{	NONE,	LEFT,	RIGHT	};



		//	Represents	size	of	gap	between	grounded	platform

		static	const	float	PLATFORM_SPACING;

		//	TODO(jarhar):	consider	removing	initial	position

		static	const	glm::vec3	INITIAL_POSITION;



		Player(glm::vec3	position	=	INITIAL_POSITION,

									glm::vec3	rotation_axis	=	glm::vec3(0,	0,	1),

									float	rotation_angle	=	0,

									glm::vec3	scale	=	glm::vec3(0.8,	0.8,	0.8));

		virtual	~Player();



		ObjectType	GetType()	override;

		SecondaryType	GetSecondaryType()	override;



		void	Reset();



		float	GetYVelocity();

		float	GetZVelocity();

		bool	GetDoubleJump();

		DuckDir	GetDucking();

		std::shared_ptr<GameObject>	GetGround();		//	null	if	no	ground

		int	GetScore();

		Animation	GetAnimation();

		std::shared_ptr<PhysicalObject>	GetRearWheel();

		std::shared_ptr<PhysicalObject>	GetFrontWheel();

		float	GetWheelRotationSpeed();

		glm::mat4	GetRotationMatrix()	const	override;

		float	GetGroundYVelocity();



		void	SetYVelocity(float	y_velocity);

		void	MoveDownZ();

		void	MoveUpZ();

		void	SetZVelocity(float	z_velocity);

		void	SetDoubleJump(bool	can_double_jump);

		void	SetDucking(DuckDir	ducking);

		void	SetGround(std::shared_ptr<GameObject>	ground);

		void	RemoveGround();

		void	SetScore(int	score);

		void	SetAnimation(Animation	animation);

		void	SetAnimationStartTick(uint64_t	animation_start_tick);

		void	SetWheelRotationSpeed(float	wheel_rotation_speed);

		void	SetCurrentTick(uint64_t	current_tick);



	private:

		std::shared_ptr<GameObject>	ground;

		bool	can_double_jump;

		float	y_velocity;

		float	z_velocity;

		int	score;



		Animation	animation;

		uint64_t	current_tick;

		uint64_t	animation_start_tick;

		uint64_t	duck_start_tick;

		float	wheel_rotation_speed;

		std::shared_ptr<PhysicalObject>	rear_wheel;

		std::shared_ptr<PhysicalObject>	front_wheel;

		DuckDir	duck_dir;

};



#endif
