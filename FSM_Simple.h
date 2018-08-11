
namespace FSMSimple {
	enum State {
		S_sleeping,
		S_hunting,
		S_eating,
		S_exploring,
	};

	struct Machine {
		State state;
		float sleepiness;
		float hunger;
		float huntTimer;
		float eatTimer;
	};

	struct Data {
		Machine machine[NUM_MACHINES];

		Data() {
			pcg32_random_t rng;
			pcg32_srandom_r(&rng, 1234, 5678);
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				M.state = S_sleeping;
				M.sleepiness = pcg32_random_r_rangef(&rng, 0.0f, 0.2f );
				M.hunger = pcg32_random_r_rangef(&rng, 0.5f, 0.9f );
				M.huntTimer = HUNTING_TIME;
				M.eatTimer = 0.0f;
			}
		}
		void Update( float deltaTime ) {
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				switch( M.state ) {
					case S_sleeping:
						{
							M.hunger += deltaTime * SLEEP_HUNGER;
							M.sleepiness += deltaTime * SLEEP_SLEEP;
							if( M.sleepiness <= 0.0f ) {
								M.sleepiness = 0.0f;
								if( M.eatTimer > 0.0f ) {
									M.state = S_eating;
								} else {
									if( M.hunger > HUNGER_TRIGGER ) {
										M.state = S_hunting;
										M.huntTimer = HUNTING_TIME;
									} else {
										M.state = S_exploring;
									}
								}
							}
						} break;
					case S_hunting:
						{
							M.hunger += deltaTime * HUNT_HUNGER;
							M.sleepiness += deltaTime * HUNT_SLEEP;
							M.huntTimer -= deltaTime;
							if( M.huntTimer <= 0.0f ) {
								M.eatTimer = EATING_TIME;
								if( M.sleepiness > SLEEP_TRIGGER ) {
									M.state = S_sleeping;
								} else {
									M.state = S_eating;
								}
							} else {
							}
						} break;
					case S_eating:
						{
							M.hunger += deltaTime * EAT_HUNGER;
							M.sleepiness += deltaTime * EAT_SLEEP;
							M.eatTimer -= deltaTime;
							if( M.sleepiness > SLEEP_TRIGGER ) {
								M.state = S_sleeping;
							} else {
								if( M.eatTimer <= 0.0f ) {
									if( M.hunger > HUNGER_TRIGGER ) {
										M.state = S_hunting;
										M.huntTimer = HUNTING_TIME;
									} else {
										M.state = S_exploring;
									}
								}
							}
						} break;
					case S_exploring:
						{
							M.hunger += deltaTime * EXPLORE_HUNGER;
							M.sleepiness += deltaTime * EXPLORE_SLEEP;
							if( M.hunger > HUNGER_TRIGGER ) {
								M.state = S_hunting;
								M.huntTimer = HUNTING_TIME;
							}
							else {
								if( M.sleepiness > SLEEP_TRIGGER ) {
									M.state = S_sleeping;
								}
							}
						} break;
				}
			}
		}
#if CHECK_FOR_CONSISTENCY
		void Report( size_t *counters ) {
			for( int i = 0; i < 4; ++ i ) counters[i] = 0;

			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				counters[M.state] += 1;
			}
		}
		void PrintState() {
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				printf( PRINT_FORMAT_STRING, "SIMPLE", STATENAME[M.state], M.sleepiness, M.hunger, M.huntTimer, M.eatTimer );
			}
		}
#endif
	};
}
