
#include <bitset>

namespace FSMVaryingTableState {
	struct HuntingMachine;
	struct Machine {
		float sleepiness;
		float hunger;
		float eatTimer;
		operator HuntingMachine ();
	};
	typedef std::vector<Machine> MachineVector;
	struct HuntingMachine : public Machine {
		float huntTimer = HUNTING_TIME;
	};
	Machine::operator HuntingMachine() {
		HuntingMachine m;
		m.sleepiness = sleepiness;
		m.hunger = hunger;
		m.eatTimer = eatTimer;
		return m;
	}
	typedef std::vector<HuntingMachine> HuntingMachineVector;

	struct Data {
		MachineVector sleeps;
		HuntingMachineVector hunts;
		MachineVector eats;
		MachineVector explores;

		Data() {
			pcg32_random_t rng;
			pcg32_srandom_r(&rng, 1234, 5678);
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine M;
				M.sleepiness = pcg32_random_r_rangef(&rng, 0.0f, 0.2f );
				M.hunger = pcg32_random_r_rangef(&rng, 0.5f, 0.9f );
				M.eatTimer = 0.0f;
				sleeps.push_back( M );
			}
		}
		void Update( float deltaTime ) {
			MachineVector pendingSleep;
			HuntingMachineVector pendingHunt;
			MachineVector pendingEat;
			MachineVector pendingExplore;

			{
				for( MachineVector::iterator iter = sleeps.begin(); iter != sleeps.end(); ) {
					Machine &M = *iter;
					M.hunger += deltaTime * SLEEP_HUNGER;
					M.sleepiness += deltaTime * SLEEP_SLEEP;
					if( M.sleepiness <= 0.0f ) {
						M.sleepiness = 0.0f;
						if( M.eatTimer > 0.0f ) {
							pendingEat.push_back(M);
						} else {
							if( M.hunger > HUNGER_TRIGGER ) {
								pendingHunt.push_back(M);
							} else {
								pendingExplore.push_back(M);
							}
						}
						//iter = sleeps.erase( iter );
						*iter = sleeps.back(); sleeps.pop_back();
					} else {
						++iter;
					}
				}
				for( HuntingMachineVector::iterator iter = hunts.begin(); iter != hunts.end(); ) {
					HuntingMachine &M = *iter;
					M.hunger += deltaTime * HUNT_HUNGER;
					M.sleepiness += deltaTime * HUNT_SLEEP;
					M.huntTimer -= deltaTime;
					if( M.huntTimer <= 0.0f ) {
						M.eatTimer = EATING_TIME;
						if( M.sleepiness > SLEEP_TRIGGER ) {
							pendingSleep.push_back(M);
						} else {
							pendingEat.push_back(M);
						}
						*iter = hunts.back(); hunts.pop_back();
					} else {
						++iter;
					}
				}
				for( MachineVector::iterator iter = eats.begin(); iter != eats.end(); ) {
					Machine &M = *iter;
					M.hunger += deltaTime * EAT_HUNGER;
					M.sleepiness += deltaTime * EAT_SLEEP;
					M.eatTimer -= deltaTime;
					if( M.sleepiness > SLEEP_TRIGGER ) {
						pendingSleep.push_back(M);
						*iter = eats.back(); eats.pop_back();
					} else {
						if( M.eatTimer <= 0.0f ) {
							if( M.hunger > HUNGER_TRIGGER ) {
								pendingHunt.push_back(M);
							} else {
								pendingExplore.push_back(M);
							} 
							//iter = eats.erase( iter );
							*iter = eats.back(); eats.pop_back();
						} else {
							++iter;
						}
					}
				}
				for( MachineVector::iterator iter = explores.begin(); iter != explores.end(); ) {
					Machine &M = *iter;
					M.hunger += deltaTime * EXPLORE_HUNGER;
					M.sleepiness += deltaTime * EXPLORE_SLEEP;
					if( M.hunger > HUNGER_TRIGGER ) {
						pendingHunt.push_back(M);
						//iter = explores.erase( iter );
						*iter = explores.back(); explores.pop_back();
					} else {
						if( M.sleepiness > SLEEP_TRIGGER ) {
							pendingSleep.push_back(M);
							//iter = explores.erase( iter );
							*iter = explores.back(); explores.pop_back();
						} else {
							++iter;
						}
					}
				}
			}

			{
				sleeps.insert( sleeps.end(), pendingSleep.begin(), pendingSleep.end() );
				hunts.insert( hunts.end(), pendingHunt.begin(), pendingHunt.end() );
				eats.insert( eats.end(), pendingEat.begin(), pendingEat.end() );
				explores.insert( explores.end(), pendingExplore.begin(), pendingExplore.end() );
				assert( sleeps.size() + hunts.size() + eats.size() + explores.size() == NUM_MACHINES );
			}
		}
#if CHECK_FOR_CONSISTENCY
		void Report( size_t *counters ) {
			counters[0] = sleeps.size();
			counters[1] = hunts.size();
			counters[2] = eats.size();
			counters[3] = explores.size();
		}
		void PrintState() {
			const char *name = "VARYING";
			for( MachineVector::iterator iter = sleeps.begin(); iter != sleeps.end(); ++iter ) {
				Machine &M = *iter;
				printf( PRINT_FORMAT_STRING, name, STATENAME[0], M.sleepiness, M.hunger, 0.0f, M.eatTimer );
			}
			for( HuntingMachineVector::iterator iter = hunts.begin(); iter != hunts.end(); ++iter ) {
				HuntingMachine &M = *iter;
				printf( PRINT_FORMAT_STRING, name, STATENAME[1], M.sleepiness, M.hunger, M.huntTimer, M.eatTimer );
			}
			for( MachineVector::iterator iter = eats.begin(); iter != eats.end(); ++iter ) {
				Machine &M = *iter;
				printf( PRINT_FORMAT_STRING, name, STATENAME[2], M.sleepiness, M.hunger, 0.0f, M.eatTimer );
			}
			for( MachineVector::iterator iter = explores.begin(); iter != explores.end(); ++iter ) {
				Machine &M = *iter;
				printf( PRINT_FORMAT_STRING, name, STATENAME[3], M.sleepiness, M.hunger, 0.0f, M.eatTimer );
			}
		}
#endif
	};
}
