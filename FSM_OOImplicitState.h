namespace FSMOOImplicitState {
	struct State;
	struct Machine {
		State *state = nullptr;
		float sleepiness;
		float hunger;
		float eatTimer;
		inline void UpdateState( State *newState );
		inline ~Machine();
	};
	struct State {
		virtual State * Update( Machine &M, float deltaTime ) = 0;
		virtual const char * GetName() { return "Base"; }
		virtual void Cleanup() {}
	};
	struct Sleeping final : public State {
		State * Update( Machine &M, float deltaTime ) override;
		const char * GetName() override { return "Sleeping"; }
	};
	struct Hunting final : public State {
		State * Update( Machine &M, float deltaTime ) override;
		const char * GetName() override { return "Hunting"; }
		float huntTimer = HUNTING_TIME;
		virtual void Cleanup() override { delete this; }
	};
	struct Eating final : public State {
		State * Update( Machine &M, float deltaTime ) override;
		virtual const char * GetName() override { return "Eating"; }
		virtual void Cleanup() override {}
	};
	struct Exploring final : public State {
		State * Update( Machine &M, float deltaTime ) override;
		const char * GetName() override { return "Exploring"; }
		virtual void Cleanup() override {}
	};

	Sleeping m_commonSleeping;
	Hunting m_commonHunting;
	Eating m_commonEating;
	Exploring m_commonExploring;

	State * GetSleeping() { return &m_commonSleeping; }
	State * GetHunting() { return new Hunting; }
	State * GetEating() { return &m_commonEating; }
	State * GetExploring() { return &m_commonExploring; }

	struct Data {
		Machine machine[NUM_MACHINES];

		Data() {
			pcg32_random_t rng;
			pcg32_srandom_r(&rng, 1234, 5678);
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				M.state = &m_commonSleeping;
				M.sleepiness = pcg32_random_r_rangef(&rng, 0.0f, 0.2f );
				M.hunger = pcg32_random_r_rangef(&rng, 0.5f, 0.9f );
				M.eatTimer = 0.0f;
			}
		}
		void Update( float deltaTime ) {
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				State *newState = M.state->Update( M, deltaTime );
				M.UpdateState(newState);
			}
		}
		int StateObjectToStateIndex( State *s ) {
			if( strcmp( s->GetName(), m_commonSleeping.GetName() ) == 0 )
				return 0;
			if( strcmp( s->GetName(), m_commonHunting.GetName() ) == 0 )
				return 1;
			if( strcmp( s->GetName(), m_commonEating.GetName() ) == 0 )
				return 2;
			if( strcmp( s->GetName(), m_commonExploring.GetName() ) == 0 )
				return 3;
			return -1;
		}
#if CHECK_FOR_CONSISTENCY
		void Report( size_t *counters ) {
			for( int i = 0; i < 4; ++ i ) counters[i] = 0;

			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				int state = StateObjectToStateIndex( M.state );
				counters[state] += 1;
			}
		}
		void PrintState() {
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				int state = StateObjectToStateIndex( M.state );
				float huntTimer = 0.0f;
				if( Hunting *h = dynamic_cast<Hunting*>( M.state ) ) {
					huntTimer = h->huntTimer;
				}
				printf( PRINT_FORMAT_STRING, "IMPLICIT", STATENAME[state], M.sleepiness, M.hunger, huntTimer, M.eatTimer );
			}
		}
#endif
	};

	// inlines
	inline void Machine::UpdateState( State *newState ) {
		if( newState ) {
			//if( state )
				//state->Cleanup();
			state = newState;
		}
	}
	inline Machine::~Machine() {
		//if( state )
			//state->Cleanup();
		state = nullptr;
	}

	State * Sleeping::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * SLEEP_HUNGER;
		M.sleepiness += deltaTime * SLEEP_SLEEP;
		if( M.sleepiness <= 0.0f ) {
			M.sleepiness = 0.0f;
			if( M.eatTimer > 0.0f ) {
				return GetEating();
			} else {
				if( M.hunger > HUNGER_TRIGGER ) {
					return GetHunting();
				} else {
					return GetExploring();
				}
			}
		}
		return nullptr;
	}
	State * Hunting::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * HUNT_HUNGER;
		M.sleepiness += deltaTime * HUNT_SLEEP;
		huntTimer -= deltaTime;
		if( huntTimer <= 0.0f ) {
			M.eatTimer = EATING_TIME;
			if( M.sleepiness > SLEEP_TRIGGER ) {
				return GetSleeping();
			} else {
				return GetEating();
			}
		}
		return nullptr;
	}
	State * Eating::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * EAT_HUNGER;
		M.sleepiness += deltaTime * EAT_SLEEP;
		M.eatTimer -= deltaTime;
		if( M.sleepiness > SLEEP_TRIGGER ) {
			return &m_commonSleeping;
		} else {
			if( M.eatTimer <= 0.0f ) {
				if( M.hunger > HUNGER_TRIGGER ) {
					return GetHunting();
				} else {
					return GetExploring();
				}
			}
		}
		return nullptr;
	}
	State * Exploring::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * EXPLORE_HUNGER;
		M.sleepiness += deltaTime * EXPLORE_SLEEP;
		if( M.hunger > HUNGER_TRIGGER ) {
			return GetHunting();
		} else {
			if( M.sleepiness > SLEEP_TRIGGER ) {
				return GetSleeping();
			}
		}
		return nullptr;
	}
}
