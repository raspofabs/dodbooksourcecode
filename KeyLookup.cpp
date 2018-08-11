#include "common.h"

int SECONDSOFANIMATION = 10;  
static const int NUM_QUERIES = 1000;
static const int NUM_NODES = 145;
static int minFrameRate = 10;
static int maxFrameRate = 15;
static const float RATIO_OF_NON_SCALING = 0.85f;

//BEGIN
// basic animation key lookup
struct FullAnimKey {
	float time;
	Vec3 translation;
	Vec3 scale;
	Vec4 rotation; // sijk quaternion
};
struct FullAnim {
	int numKeys;
	FullAnimKey *keys;
	FullAnimKey GetKeyAtTimeBinary( float t ) {
		int l = 0, h = numKeys-1;
		int m = (l+h) / 2;
		while( l < h ) {
			if( t < keys[m].time ) {
				h = m-1;
			} else {
				l = m;
			}
			m = (l+h+1) / 2;
		}
		return keys[m];
	}
	FullAnimKey GetKeyAtTimeLinear( float t ) {
		int i = 0;
		while( i < numKeys ) {
			if( keys[i].time > t ) {
				--i;
				break;
			}
			++i;
		}
		if( i < 0 )
			return keys[0];
		return keys[i];
	}
};

// looking up keys by time
struct DataOnlyAnimKey {
	Vec3 translation;
	Vec3 scale;
	Vec4 rotation; // sijk quaternion
};
struct DataOnlyAnim {
	int numKeys;
	float *keyTime;
	DataOnlyAnimKey *keys;
	DataOnlyAnimKey GetKeyAtTimeBinary( float t ) {
		int l = 0, h = numKeys-1;
		int m = (l+h) / 2;
		while( l < h ) {
			if( t < keyTime[m] ) {
				h = m-1;
			} else {
				l = m;
			}
			m = (l+h+1) / 2;
		}
		return keys[m];
	}
	DataOnlyAnimKey GetKeyAtTimeLinear( float t ) {
		int i = 0;
		while( i < numKeys ) {
			if( keyTime[i] > t ) {
				--i;
				break;
			}
			++i;
		}
		if( i < 0 )
			return keys[0];
		return keys[i];
	}
};
struct ClumpedAnim {
	int numKeys;
	float *keyTime;
	DataOnlyAnimKey *keys;
	static const int numPrefetchedKeyTimes = (64-sizeof(int)-sizeof(float*)-sizeof(DataOnlyAnimKey*))/sizeof(float);
	static const int keysPerLump = 64/sizeof(float);
	float firstStage[numPrefetchedKeyTimes];
	DataOnlyAnimKey GetKeyAtTimeBinary( float t ) {
		for( int start = 0; start < numPrefetchedKeyTimes; ++start ) {
			if( firstStage[start] > t ) {
				int l = start*keysPerLump;
				int h = l + keysPerLump;
				h = h > numKeys ? numKeys : h;
				return GetKeyAtTimeBinary( t, l, h+1 );
			}
		}
		return GetKeyAtTimeBinary( t, numPrefetchedKeyTimes*keysPerLump, numKeys );
	}
	DataOnlyAnimKey GetKeyAtTimeBinary( float t, int l, int h ) {
		int m = (l+h) / 2;
		while( l < h ) {
			if( t < keyTime[m] ) {
				h = m-1;
			} else {
				l = m;
			}
			m = (l+h+1) / 2;
		}
		return keys[m];
	}
	DataOnlyAnimKey GetKeyAtTimeLinear( float t ) {
		for( int start = 0; start < numPrefetchedKeyTimes; ++start ) {
			if( firstStage[start] > t ) {
				int l = start*keysPerLump;
				int h = l + keysPerLump;
				h = h > numKeys ? numKeys : h;
				return GetKeyAtTimeLinear( t, l );
			}
		}
		return GetKeyAtTimeLinear( t, numPrefetchedKeyTimes*keysPerLump );
	}
	DataOnlyAnimKey GetKeyAtTimeLinear( float t, int startIndex ) {
		int i = startIndex;
		while( i < numKeys ) {
			if( keyTime[i] > t ) {
				--i;
				break;
			}
			++i;
		}
		if( i < 0 )
			return keys[0];
		return keys[i];
	}
};

//END

struct TranslationKey { float t; Vec3 pos; };
struct RotationKey { float t; Vec4 quat; };
struct ScaleKey { float t; Vec3 scale; };

typedef std::vector<TranslationKey> TKVec;
typedef std::vector<RotationKey> RKVec;
typedef std::vector<ScaleKey> SKVec;
struct AnimData {
	TKVec tKeys;
	RKVec rKeys;
	SKVec sKeys;
};


AnimData PrepareData( uint32_t seed = 1234, bool bTranslationAnimation = false, bool bScaleAnimation = false ) {
	TKVec tKeys;
	RKVec rKeys;
	SKVec sKeys;

	pcg32_random_t rng;
	pcg32_srandom_r(&rng, seed, 5678);
	
	// Make a ten second long animation, with keys at various times
	int secondsOfAnimation = SECONDSOFANIMATION;

	// How many keys? At 60fps, motion capture can get very high framerates, but
	// maybe about 10-15 keyframes per second is about right.
	int keyCount = pcg32_random_r_range(&rng,
			minFrameRate*secondsOfAnimation,
			maxFrameRate*secondsOfAnimation);

	//printf( "Keys[%i] %s translation, %s scale\n",
			//keyCount,
			//bTranslationAnimation?"with":"without",
			//bScaleAnimation?"with":"without"
			//);
#ifndef NDEBUG
	int sharedKeys = 1;
	if( bTranslationAnimation || bScaleAnimation ) {
		// if there is animation on the translation or scale components, then
		// somewhere between 2 and all of the keyframes will have the same timing.
		// (always share first and last keyframes times)
		sharedKeys = pcg32_random_r_range(&rng, 2, keyCount+1);
		assert( sharedKeys <= keyCount ); // just ensuring my random gen is actually behaving
	}
#endif
	
	// generate some keyframe data
	for( int i = 0; i < keyCount; ++i ) {
		Vec4 r; // simulate a quaternion
		r.x = pcg32_random_r_rangef(&rng, -1, 1);
		r.y = pcg32_random_r_rangef(&rng, -1, 1);
		r.z = pcg32_random_r_rangef(&rng, -1, 1);
		r.w = 1.0f - sqrt( dot(r,r));
		rKeys.push_back(RotationKey{-1,r});
		if( i == 0 || bTranslationAnimation ) {
			Vec3 t;
			t.x = pcg32_random_r_rangef(&rng, -10, 10);
			t.y = pcg32_random_r_rangef(&rng, -10, 10);
			t.z = pcg32_random_r_rangef(&rng, -10, 10);
			tKeys.push_back(TranslationKey{-1,t});
		}
		if( i == 0 || bScaleAnimation ) {
			Vec3 s;
			s.x = pcg32_random_r_rangef(&rng, 0.1f, 2.0f);
			s.y = pcg32_random_r_rangef(&rng, 0.1f, 2.0f);
			s.z = pcg32_random_r_rangef(&rng, 0.1f, 2.0f);
			sKeys.push_back(ScaleKey{-1,s});
		}
	}

	if(!bTranslationAnimation) {
		// set an initial keyframe at time 0
		tKeys[0].t = 0;
	}
	if(!bScaleAnimation) {
		// set an initial keyframe at time 0
		sKeys[0].t = 0;
	}

	std::vector<float> keyTimes;
	keyTimes.push_back(0.0f);
	keyTimes.push_back(secondsOfAnimation);
	// now generate some basic key times
	for( int i = 2; i < keyCount; ++i ) {
		keyTimes.push_back(
				pcg32_random_r_rangef(&rng, 0.1f, secondsOfAnimation -0.1f)
				); // arbitrary reduction of range
	}
	std::sort(keyTimes.begin(), keyTimes.end());
	// apply these times to the rotation keys
	for( int i = 0; i < keyCount; ++i ) {
		rKeys[i].t = keyTimes[i];
		if( bTranslationAnimation ) {
			tKeys[i].t = keyTimes[i];
		}
		if( bScaleAnimation ) {
			sKeys[i].t = keyTimes[i];
		}
	}

#if 0 // unsharing the key times
	int unsharedKeys = keyCount - sharedKeys;
	for( int i = 0; i < unsharedKeys; ++i ) {
		int keyToJitter = pcg32_random_r_range(&rng, 1, keyCount-1 ); // any key between the start and end

		// we're only after making sure they aren't the exact same time value, so just jitter them.
		if( bTranslationAnimation ) {
			tKeys[i].t = pcg32_random_r_rangef(&rng, tKeys[i-1].t, tKeys[i+1].t );
		}
		if( bScaleAnimation ) {
			sKeys[i].t = pcg32_random_r_rangef(&rng, sKeys[i-1].t, sKeys[i+1].t );
		}
	}
#endif

#ifndef NDEBUG
	// check we did all good
	for( auto r : rKeys ) {
		assert( r.t >= 0.0f );
		assert( r.t <= secondsOfAnimation );
	}
	for( auto t : tKeys ) {
		assert( t.t >= 0.0f );
		assert( t.t <= secondsOfAnimation );
	}
	for( auto s : sKeys ) {
		assert( s.t >= 0.0f );
		assert( s.t <= secondsOfAnimation );
	}
#endif
	return AnimData { tKeys, rKeys, sKeys, };
}

void FromData( FullAnim &out, const AnimData &in ) {
	std::vector<FullAnimKey> outkeys;
	for( size_t i = 0; i < in.rKeys.size(); ++i ) {
		auto r = in.rKeys[i];
		auto t = in.tKeys.size()==1?in.tKeys[0]:in.tKeys[i];
		auto s = in.sKeys.size()==1?in.sKeys[0]:in.sKeys[i];
		FullAnimKey ak;
		ak.time = r.t;
		ak.rotation = r.quat;
		ak.translation = t.pos;
		ak.scale = s.scale;
		outkeys.push_back( ak );
	}
	out.keys = new FullAnimKey[outkeys.size()];
	std::copy( outkeys.begin(), outkeys.end(), out.keys );
	out.numKeys = outkeys.size();
}

void FromData( DataOnlyAnim &out, const AnimData &in ) {
	std::vector<DataOnlyAnimKey> outkeys;
	std::vector<float> times;
	for( size_t i = 0; i < in.rKeys.size(); ++i ) {
		auto r = in.rKeys[i];
		auto t = in.tKeys.size()==1?in.tKeys[0]:in.tKeys[i];
		auto s = in.sKeys.size()==1?in.sKeys[0]:in.sKeys[i];
		times.push_back( r.t );
		DataOnlyAnimKey ak;
		ak.rotation = r.quat;
		ak.translation = t.pos;
		ak.scale = s.scale;
		outkeys.push_back( ak );
	}
	out.keys = new DataOnlyAnimKey[outkeys.size()];
	std::copy( outkeys.begin(), outkeys.end(), out.keys );
	out.keyTime = new float[times.size()];
	std::copy( times.begin(), times.end(), out.keyTime );
	out.numKeys = outkeys.size();
}
void FromData( ClumpedAnim &out, const AnimData &in ) {
	std::vector<DataOnlyAnimKey> outkeys;
	std::vector<float> times;
	for( size_t i = 0; i < in.rKeys.size(); ++i ) {
		auto r = in.rKeys[i];
		auto t = in.tKeys.size()==1?in.tKeys[0]:in.tKeys[i];
		auto s = in.sKeys.size()==1?in.sKeys[0]:in.sKeys[i];
		times.push_back( r.t );
		DataOnlyAnimKey ak;
		ak.rotation = r.quat;
		ak.translation = t.pos;
		ak.scale = s.scale;
		outkeys.push_back( ak );
	}
	out.keys = new DataOnlyAnimKey[outkeys.size()];
	std::copy( outkeys.begin(), outkeys.end(), out.keys );
	out.keyTime = new float[times.size()];
	std::copy( times.begin(), times.end(), out.keyTime );
	out.numKeys = outkeys.size();
	// fill out the precached times
	for( size_t i = 0; i < ClumpedAnim::numPrefetchedKeyTimes; ++i ) {
		size_t targetIndex = (i+1)*ClumpedAnim::keysPerLump;
		if( targetIndex < times.size() ) {
			out.firstStage[i] = times[targetIndex];
		} else {
			out.firstStage[i] = HUGE_VALF;
		}
	}
}

struct HierarchyOutputData {
	struct NodeData {
		Vec3 translation;
		Vec3 scale;
		Vec4 rotation; // sijk quaternion
	};
	NodeData nodeData[NUM_NODES];
};
template<typename AnimType>
struct TestHierarchy {
	AnimType animForNode[NUM_NODES];
	void SetupNode( int node, const AnimData &ad ) {
		FromData( animForNode[node], ad );
	}
	HierarchyOutputData GetAtTBinary( float t ) {
		HierarchyOutputData hod;
		for( int i = 0; i < NUM_NODES; ++i ) {
			auto keyData = animForNode[i].GetKeyAtTimeBinary( t );
			hod.nodeData[i].translation = keyData.translation;
			hod.nodeData[i].rotation = keyData.rotation;
			hod.nodeData[i].scale = keyData.scale;
		}
		return hod;
	}
	HierarchyOutputData GetAtTLinear( float t ) {
		HierarchyOutputData hod;
		for( int i = 0; i < NUM_NODES; ++i ) {
			auto keyData = animForNode[i].GetKeyAtTimeLinear( t );
			hod.nodeData[i].translation = keyData.translation;
			hod.nodeData[i].rotation = keyData.rotation;
			hod.nodeData[i].scale = keyData.scale;
		}
		return hod;
	}
};

volatile HierarchyOutputData output_data;
struct Data {
	TestHierarchy<FullAnim> fullAnimHierarchy;
	TestHierarchy<DataOnlyAnim> dataOnlyHierarchy;
	TestHierarchy<ClumpedAnim> clumpedHierarchy;
	std::vector<float> queries;

	Data() {
		AnimData ad = PrepareData( 1000, true, false );
		fullAnimHierarchy.SetupNode( 0, ad );
		dataOnlyHierarchy.SetupNode( 0, ad );
		clumpedHierarchy.SetupNode( 0, ad );
		for( int i = 1; i < NUM_NODES; ++i ) {
			// let's have some scaling on the last few nodes
			bool bScaled = i>(RATIO_OF_NON_SCALING*NUM_NODES);
			ad = PrepareData( 1000+i, false, bScaled );
			fullAnimHierarchy.SetupNode( i, ad );
			dataOnlyHierarchy.SetupNode( i, ad );
			clumpedHierarchy.SetupNode( i, ad );
		}
		{
			pcg32_random_t rng;
			pcg32_srandom_r(&rng, 1234312, 55544);
			for( int i = 0; i < NUM_QUERIES; ++i ) {
				float t = pcg32_random_r_rangef(&rng, -0.5f, SECONDSOFANIMATION + 0.5f );
				queries.push_back( t );
			}
		}
	}
};
Data *gData;
void TestFullAnimBinary() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->fullAnimHierarchy.GetAtTBinary(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}
void TestFullAnimLinear() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->fullAnimHierarchy.GetAtTLinear(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}
void TestDataOnlyBinary() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->dataOnlyHierarchy.GetAtTBinary(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}
void TestDataOnlyLinear() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->dataOnlyHierarchy.GetAtTLinear(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}
void TestClumpedBinary() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->clumpedHierarchy.GetAtTBinary(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}
void TestClumpedLinear() {
	for( auto t : gData->queries ) {
		HierarchyOutputData hod = gData->clumpedHierarchy.GetAtTLinear(t);
		memcpy( (void*)&output_data, &hod, sizeof( output_data ) );
	}
}


int main() {
	Data data; gData = &data;

	Test tests[] = {
		(Test){ TestFullAnimBinary, "Full anim key - binary search" },
		(Test){ TestDataOnlyBinary, "Data only key - binary search" },
		(Test){ TestClumpedBinary, "Pre-indexed - binary search" },
		(Test){ TestFullAnimLinear, "Full anim key - linear search" },
		(Test){ TestDataOnlyLinear, "Data only key - linear search" },
		(Test){ TestClumpedLinear, "Pre-indexed - linear search" },
	};

	printf( "Animation key lookup tests\n" );
	printf( "Clumped precached keys = %i clumps of %i keys (max key = %i)\n\n", ClumpedAnim::numPrefetchedKeyTimes, ClumpedAnim::keysPerLump, ClumpedAnim::numPrefetchedKeyTimes * ClumpedAnim::keysPerLump );
	printf( "Each hierarchy has %i nodes.\n", NUM_NODES );
	printf( "Each hierarchy is queried %i times\n", NUM_QUERIES );

	RunTests( tests );

	return 0;
}
