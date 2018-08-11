#include "common.h"
#include <map>

struct B {
	int height;
	bool isClockwise;
};

struct A {
	bool canStandOnOneLeg;
	bool hasTheirOwnHair;
	bool ownsADog;
	bool isOwnedByACat;

	// cached "has B info"
	bool isCached25 : 1;
	bool isCached50 : 1;
	bool isCached75 : 1;
	bool isCached95 : 1;
	bool isCached99 : 1;
	bool hasBInfo : 1;
	bool isTall : 1;
};

static std::map<int,B> BInfoMap;
static std::vector<A> AInfoVec;

static const int NUM_IN_TEST = 128 * 1024;

void Setup() {
	Timer t;
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	AInfoVec.clear();
	BInfoMap.clear();
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A a;
		a.canStandOnOneLeg = pcg32_random_r_probability(&rng, 0.99f);
		a.hasTheirOwnHair = pcg32_random_r_probability(&rng, 0.90f);
		a.ownsADog = pcg32_random_r_probability(&rng, 0.46f);
		a.isOwnedByACat = pcg32_random_r_probability(&rng, 0.42f);
		a.isCached25 = pcg32_random_r_probability(&rng, 0.25f);
		a.isCached50 = pcg32_random_r_probability(&rng, 0.5f);
		a.isCached75 = pcg32_random_r_probability(&rng, 0.75f);
		a.isCached95 = pcg32_random_r_probability(&rng, 0.95f);
		a.isCached99 = pcg32_random_r_probability(&rng, 0.99f);
		a.hasBInfo = pcg32_random_r_probability(&rng, 0.25f);
		if( a.hasBInfo ) {
			B b;
			b.height = pcg32_random_r_range(&rng, 150, 200);
			b.isClockwise = pcg32_random_r_probability(&rng, 0.5f);
			BInfoMap[i] = b;
			a.isTall = b.height > 185;
		}
		AInfoVec.push_back( a );
	}
	printf( "Setup took %fms\n", t.elapsed() );
}

std::pair<int,int> Simple() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( BInfoMap.find( i ) != BInfoMap.end() ) {
				if( BInfoMap[i].height > 185 ) {
					taller += 1;
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}

std::pair<int,int> Bool() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.hasBInfo ) {
				if( BInfoMap[i].height > 185 ) {
					taller += 1;
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}

std::pair<int,int> CachedBool50() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached50 ) {
				if( a.hasBInfo ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}

std::pair<int,int> Cached() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.hasBInfo && a.isTall ) {
				taller += 1;
			}
		}
	}
	return std::pair<int,int>(good,taller);
}

std::pair<int,int> PartiallyCached25() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached25 ) {
				if( a.hasBInfo && a.isTall ) {
					taller += 1;
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}
std::pair<int,int> PartiallyCached50() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached50 ) {
				if( a.hasBInfo && a.isTall ) {
					taller += 1;
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}
std::pair<int,int> PartiallyCached75() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached75 ) {
				if( a.hasBInfo && a.isTall ) {
					taller += 1;
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}
std::pair<int,int> PartiallyCached95() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached95 ) {
				if( a.hasBInfo && a.isTall ) {
					taller += 1;
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}
std::pair<int,int> PartiallyCached99() {
	int good = 0;
	int taller = 0;
	for( int i = 0; i < NUM_IN_TEST; ++i ) {
		A &a = AInfoVec[i];
		if( a.canStandOnOneLeg && a.hasTheirOwnHair ) {
			good += 1;
			if( a.isCached99 ) {
				if( a.hasBInfo && a.isTall ) {
					taller += 1;
				}
			} else {
				if( BInfoMap.find( i ) != BInfoMap.end() ) {
					if( BInfoMap[i].height > 185 ) {
						taller += 1;
					}
				}
			}
		}
	}
	return std::pair<int,int>(good,taller);
}

int validCount[16];
std::pair<int,int> valid;

void TestSimple() {
	std::pair<int,int> result = Simple();
	if( valid.first == result.first && valid.second == result.second )
		validCount[0] += 1;
}
void TestBool() {
	std::pair<int,int> result = Bool();
	if( valid.first == result.first && valid.second == result.second )
		validCount[1] += 1;
}
void TestCachedBool() {
	std::pair<int,int> result = CachedBool50();
	if( valid.first == result.first && valid.second == result.second )
		validCount[1] += 1;
}
void TestCached() {
	std::pair<int,int> result = Cached();
	if( valid.first == result.first && valid.second == result.second )
		validCount[2] += 1;
}
void TestPartiallyCached25() {
	std::pair<int,int> result = PartiallyCached25();
	if( valid.first == result.first && valid.second == result.second )
		validCount[3] += 1;
}
void TestPartiallyCached50() {
	std::pair<int,int> result = PartiallyCached50();
	if( valid.first == result.first && valid.second == result.second )
		validCount[4] += 1;
}
void TestPartiallyCached75() {
	std::pair<int,int> result = PartiallyCached75();
	if( valid.first == result.first && valid.second == result.second )
		validCount[5] += 1;
}
void TestPartiallyCached95() {
	std::pair<int,int> result = PartiallyCached95();
	if( valid.first == result.first && valid.second == result.second )
		validCount[5] += 1;
}
void TestPartiallyCached99() {
	std::pair<int,int> result = PartiallyCached99();
	if( valid.first == result.first && valid.second == result.second )
		validCount[5] += 1;
}


int main() {

	Test tests[] = {
		(Test){ TestSimple, "Simple, check the map" },
		(Test){ TestBool, "Simple, cache presence" },
		(Test){ TestCachedBool, "Partially cached presence (50%)" },
		(Test){ TestCached, "Fully cached query" },
		(Test){ TestPartiallyCached25, "Partially cached query (25%)" },
		(Test){ TestPartiallyCached50, "Partially cached query (50%)" },
		(Test){ TestPartiallyCached75, "Partially cached query (75%)" },
		(Test){ TestPartiallyCached95, "Partially cached query (95%)" },
		(Test){ TestPartiallyCached99, "Partially cached query (99%)" },
	};

	Setup();

	valid = Simple();

	RunTests( tests );

	return 0;
}
