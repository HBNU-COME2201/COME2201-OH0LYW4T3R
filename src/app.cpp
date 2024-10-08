#include <iostream>
#include "agent.hpp"
#include "mmanager.hpp"
#include "stochastic_decorator.hpp"
#include "simple_decorator.hpp"

#include <vector>
#include "tinyxml2.h"
#include <string>
#include "special_agent.hpp"

// 재사용성을 중심으로 주석 달것

class InitManager // 팩토리 역할을 해줌
{
public:
    InitManager() {}
    InitManager(std::string path) 
    {
        tinyxml2::XMLDocument doc; // xml 도큐먼트를 열게 하기 위해 변수 선언
        doc.LoadFile(path.c_str()); // 매개변수로 받은 string값으로 파일을 열음
        //현재 xml 계층 구조가 -> scenario -> AgentList -> agent 이므로 AgentList까지 이동하는 코드
        tinyxml2::XMLElement* agListElem = doc.FirstChildElement( "scenario" )->FirstChildElement( "AgentList" );
        
        // 클린룸 어프로치 (문제없는 부분은 빼고-하나씩 클린하게 만들어감)
        double x, y, heading, speed, drange; 
        for (tinyxml2::XMLElement* agElem = agListElem->FirstChildElement();
            agElem != NULL; agElem = agElem->NextSiblingElement()) // agent를 마지막까지 훑는 코드
            {
                // xml의 agent의 Attribute를 가져오는 코드
                agElem->QueryDoubleAttribute("x", &x);
                agElem->QueryDoubleAttribute("y", &y);
                agElem->QueryDoubleAttribute("heading", &heading);
                agElem->QueryDoubleAttribute("speed", &speed);
                agElem->QueryDoubleAttribute("drange", &drange);
                // xml의 agent의 Attribute를 가져오는 코드

                // 다음 코드는 CAgent를 상속받은 CSpecialAgent가 다형성을 보여주고 있는 코드이며
                // 클라이언트코드는 m_agent_list를 CAgent* 타입으로 뽑기만 하면된다. (자식 타입은 부모 타입으로 타입 변환이 되므로 클라이언트는 신경쓰지 않아도 됨.)
                // 그래서 다양하게 CAgent를 상속받아 자신이 원하는 비지니스 로직으로 변환할 수 있고 클라이언트는 신경쓰지 않고 각 자식 특징에 맞게 코드가 출력이 된다.
                CAgent* ag = new CSpecialAgent(x, y, heading, speed, drange); 
                // agent를 m_agent_list뒤에 넣음
                m_agent_list.push_back(ag);
            }   
    }
private:
    std::vector<CAgent*> m_agent_list;

public:
    std::vector<CAgent*>& get_agent_list() { return m_agent_list; }
};

int main(int argc, char** argv)
{// Clinet Code  
    
    InitManager init_manager("test.xml"); // 팩토리 선언

    /* Simulation Engine */
    CManeuverManager mmanager;

    for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); iter++)
        {
            mmanager.register_publisher(*iter); // 제공자 등록
        }
    // tinyxml2::XMLElement* attributeApproachElement = doc.FirstChildElement()->FirstChildElement( "attributeApproach" );
	// attributeApproachElement->QueryIntAttribute( "v", &v0 );

    // std::vector<CAgent*> m_agent_list;
    // // CAgent* p_ag = ??? create_agent(10, 0, 0, 1, 10);
    // // m_agent_list.push_back(p_ag); 뒤에다가 추가해라


    // /* Simulation Model Registration */
    // // 문제가 되는 부분 (agent 여러명이면?, 계속 새롭게 컴파일 해야함)
    // CAgent* p_agent1 = new CAgent(10,  0, 0, 1, 10);
    // p_agent1 = new StochasticDecorator(p_agent1);
    // p_agent1 = new SimpleDecorator(p_agent1);
    
    // mmanager.register_publisher(p_agent1);

    // CAgent* p_agent2 = new CAgent(10,  30, 180, 1, 10);
    // mmanager.register_publisher(p_agent2);
    // // 문제가 되는 부분
    
    /* Simulation Engine Initialization */
    double sim_time;
    double time_step = 1;

    for(sim_time = 0; sim_time < 30; sim_time += time_step)
    {   
        //p_agent1->maneuver(time_step);
        //p_agent2->maneuver(time_step);

        //p_agent1->detect(p_agent2);
        //p_agent2->detect(p_agent1);

        mmanager.svc(time_step); //서비스

        // std::cout << "----" << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent1 << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent2 << std::endl;

        std::cout << "----" << std::endl;
        for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); iter++)
        {
            std::cout << "Time: " << sim_time << ", " << *(*iter) << std::endl; // 변수의 이름으로 별도로 지정해줄 필요가 없음 iter 참조하면 됨
            // 정보는 모두 text.xml에 있음
        }
    }
    return 0;
}
