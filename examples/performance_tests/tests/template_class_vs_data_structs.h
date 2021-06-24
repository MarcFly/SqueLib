#ifndef _TEST_TEMPLATE_VS_STRUCT_
#define _TEST_TEMPLATE_VS_STRUCT_

class component
{
public:
	component() {};
	component(uint32_t _type) : type(_type) {};
	virtual ~component() {};

	uint32_t type;
	uint32_t ref;
};

sque_vec<component> components;

template<class T>
void AddComponent(uint32_t entity_ref)
{
	//component* c = &T().c;
	components.push_back(T::Create());
}

template<class T>
void GetComponent(uint32_t entity_ref)
{
	for (uint32_t i = 0; i < components.size(); ++i)
		if (components[i].type == T::type)
		{
			T::GetByRef(components[i].ref);
			return;
		}
}

class sub1
{
public:
	static component Create();
	static sub1& GetByRef(uint32_t ref);
	static const uint32_t type = 1;

	sub1() {};
	~sub1() {};

	float p[3];
	float r[4];
	float s[3];
};
sque_vec<sub1> comp1;
component sub1::Create()
{
	comp1.push_back(sub1());
	component ret;
	ret.ref = comp1.size() - 1;
	ret.type = type;
	return ret;
}

sub1& sub1::GetByRef(uint32_t ref)
{
	return comp1[ref];
}


class sub2
{
public:
	static component Create();
	static sub2& GetByRef(uint32_t ref);
	static const uint32_t type = 2;

	sub2() {};
	~sub2() {};

	float m[16];
	float d[10];
	uint32_t id;
	uint32_t id2;
};
sque_vec<sub2> comp2;
component sub2::Create()
{
	comp2.push_back(sub2());
	component ret;
	ret.ref = comp2.size() - 1;
	ret.type = type;
	return ret;
}
sub2& sub2::GetByRef(uint32_t ref)
{
	return comp2[ref];
}

// Struct Based - There is a lot of code repetition for generating the component
// You will have to specifically invoke different function names AddComp1, AddComp2,... with same values



struct component_d
{
	uint32_t type;
	uint32_t ref;
};
sque_vec<component_d> components_d;

void AddComponent_D(const uint32_t entity_ref, component_d comp)
{
	components_d.push_back(comp);
	// In real code it goes to the component reference vecrtor of vectors 
	// and is pushed to the vector of the entity, but you have to call first teh component function to add!
}

uint32_t GetComponentRef_D(const uint32_t entity_ref, uint32_t type)
{
	sque_vec<component_d>& comps = components_d;
	for (uint32_t i = 0; i < comps.size(); ++i)
	{
		if (comps[i].type == type)
			return comps[i].ref;
	}

	return -1;
}

struct sub1_d
{
	float p[3];
	float r[4];
	float s[3];
};
sque_vec<sub1_d> comp1_d;

component_d Add1()
{
	component_d ret;
	sub1_d new_sub1_d;
	ret.ref = comp1_d.try_insert(new_sub1_d);
	ret.type = 1;

	return ret;
}

sub1_d GetSub1_D(uint32_t ref)
{
	return comp1_d[ref];
}



struct sub2_d
{
	float m[16];
	float d[10];
	uint32_t id;
	uint32_t id2;
};
sque_vec<sub2_d> comp2_d;

component_d Add2()
{
	component_d ret;
	sub2_d new_sub1_d;
	ret.ref = comp2_d.try_insert(new_sub1_d);
	ret.type = 2;

	return ret;
}

sub2_d GetSub2_D(uint32_t ref)
{
	return comp2_d[ref];
}

sque_vec<double> template_times;
sque_vec<double> data_struct_times;

void Test_Template_VS_Struct()
{
	SQUE_Timer t_;

	SQUE_PRINT(LT_INFO, "ADD 1mill Components Type 1/2 randomly");
	SQUE_PRINT(LT_INFO, "Get 1mill Components 1/2 randomly");
	components.reserve(TIMES);
	comp1.reserve(TIMES);
	comp2.reserve(TIMES);
	components_d.reserve(TIMES);
	comp2_d.reserve(TIMES);
	comp1_d.reserve(TIMES);

	t_.Start();

	for (uint32_t i = 0; i < TIMES; ++i)
		if(SQUE_RNG(100) > 50)
			AddComponent<sub1>(0);
		else
			AddComponent<sub2>(0);

	for (uint32_t i = 0; i < TIMES; ++i)
	{
		if (SQUE_RNG(100) > 50)
			GetComponent<sub1>(0);
		else 
			GetComponent<sub2>(0);

	}
	t_.Stop();
	SQUE_PRINT(LT_INFO, "Template Class took: %lf ms", t_.ReadMilliSec());
	template_times.push_back(t_.ReadMicroSec());


	t_.Start();
	for (uint32_t i = 0; i < TIMES; ++i)
		if (SQUE_RNG(100) > 50)
			AddComponent_D(0, Add1());
		else
			AddComponent_D(0, Add2());

	for (uint32_t i = 0; i < TIMES; ++i)
	{
		if (SQUE_RNG(100) > 50)
			GetSub1_D(GetComponentRef_D(0, 1));
		else
			GetSub2_D(GetComponentRef_D(0, 2));

	}
	t_.Stop();
	SQUE_PRINT(LT_INFO, "Data Struct took: %lf ms", t_.ReadMilliSec());
	data_struct_times.push_back(t_.ReadMicroSec());

	components.clear();
	comp1.clear();
	comp2.clear();

	components_d.clear();
	comp1_d.clear();
	comp2_d.clear();

}


void TestAverageTimes_Template_Struct()
{
	double template_avg = 0, struct_avg = 0;
	for (uint16_t i = 0; i < template_times.size(); ++i)
	{
		template_avg += template_times[i];
		struct_avg += data_struct_times[i];
	}
	template_avg /= (double)template_times.size() * 1000.;
	struct_avg /= (double)data_struct_times.size() * 1000.;
	SQUE_PRINT(LT_INFO, "Average Template Time: %lf millisec", template_avg);
	SQUE_PRINT(LT_INFO, "Average Data Struct Time: %lf millisec", struct_avg);

	template_times.clear();
	data_struct_times.clear();
}

#endif