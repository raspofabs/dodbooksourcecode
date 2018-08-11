
namespace FSMOOState {
	struct State;
	struct Machine {
		State *state = nullptr;
		float sleepiness;
		float hunger;
		float huntTimer;
		float eatTimer;
		inline void UpdateState( State *newState );
		inline ~Machine();
	};
	struct State {
		virtual State * Update( Machine &M, float deltaTime ) = 0;
	};
	struct Sleeping final : public State {
		virtual State * Update( Machine &M, float deltaTime ) override;
	};
	struct Hunting final : public State {
		virtual State * Update( Machine &M, float deltaTime ) override;
	};
	struct Eating final : public State {
		virtual State * Update( Machine &M, float deltaTime ) override;
	};
	struct Exploring final : public State {
		virtual State * Update( Machine &M, float deltaTime ) override;
	};

	Sleeping m_commonSleeping;
	Hunting m_commonHunting;
	Eating m_commonEating;
	Exploring m_commonExploring;

	struct Data {
		Machine machine[NUM_MACHINES];

		Data() {
			pcg32_random_t rng;
			pcg32_srandom_r(&rng, 1234, 5678);
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				M.state = new Sleeping;
				M.sleepiness = pcg32_random_r_rangef(&rng, 0.0f, 0.2f );
				M.hunger = pcg32_random_r_rangef(&rng, 0.5f, 0.9f );
				M.huntTimer = HUNTING_TIME;
				M.eatTimer = EATING_TIME;
			}
		}
		void Update( float deltaTime ) {
			for( int m = 0; m < NUM_MACHINES; ++m ) {
				Machine &M = machine[m];
				M.UpdateState( M.state->Update( M, deltaTime ) );
			}
		}
	};

	// inlines
	inline void Machine::UpdateState( State *newState ) {
		if( newState ) {
			//if( state )
				//delete state;
			state = newState;
		}
	}
	inline Machine::~Machine() {
		//if( state )
			//delete state;
		state = nullptr;
	}

	State * Sleeping::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * SLEEP_HUNGER;
		M.sleepiness += deltaTime * SLEEP_SLEEP;
		if( M.sleepiness <= 0.0f ) {
			M.sleepiness = 0.0f;
			if( M.hunger > HUNGER_TRIGGER ) {
				return new Hunting;
			} else {
				return new Exploring;
			}
		}
	}
	State * Hunting::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * HUNT_HUNGER;
		M.sleepiness += deltaTime * HUNT_SLEEP;
		if( M.sleepiness > SLEEP_TRIGGER ) {
			return new Sleeping;
		}
		if( M.huntTimer <= 0.0f ) {
			M.huntTimer = HUNTING_TIME;
			M.eatTimer = EATING_TIME;
			return new Eating;
		}
	}
	State * Eating::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * EAT_HUNGER;
		M.sleepiness += deltaTime * EAT_SLEEP;
		M.eatTimer -= deltaTime;
		if( M.eatTimer <= 0.0f ) {
			if( M.hunger > HUNGER_TRIGGER ) {
				return new Hunting;
			} else {
				return new Exploring;
			}
		}
	}
	State * Exploring::Update( Machine &M, float deltaTime ) {
		M.hunger += deltaTime * EXPLORE_HUNGER;
		M.sleepiness += deltaTime * EXPLORE_SLEEP;
		if( M.hunger > HUNGER_TRIGGER ) {
			return new Hunting;
		}
		if( M.sleepiness > SLEEP_TRIGGER ) {
			return new Sleeping;
		}
	}
}
