/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima RTPS is licensed to you under the terms described in the
 * fastrtps_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file ParticipantImpl.cpp
 *
 */

#include "fastrtps/participant/ParticipantImpl.h"
#include "fastrtps/participant/Participant.h"
#include "fastrtps/participant/ParticipantDiscoveryInfo.h"
#include "fastrtps/participant/ParticipantListener.h"

#include "fastrtps/TopicDataType.h"

#include "fastrtps/rtps/participant/RTPSParticipant.h"

#include "fastrtps/attributes/PublisherAttributes.h"
#include "fastrtps/publisher/PublisherImpl.h"
#include "fastrtps/publisher/Publisher.h"

#include "fastrtps/attributes/SubscriberAttributes.h"
#include "fastrtps/subscriber/SubscriberImpl.h"
#include "fastrtps/subscriber/Subscriber.h"

#include "fastrtps/rtps/RTPSDomain.h"

#include "fastrtps/utils/RTPSLog.h"

using namespace eprosima::fastrtps::rtps;

namespace eprosima {
namespace fastrtps {

static const char* const CLASS_NAME = "ParticipantImpl";

ParticipantImpl::ParticipantImpl(ParticipantAttributes& patt,Participant* pspart,ParticipantListener* listen):
		m_att(patt),
		mp_rtpsParticipant(nullptr),
		mp_participant(pspart),
		mp_listener(listen),
#pragma warning (disable : 4355 )
		m_rtps_listener(this)
{
	mp_participant->mp_impl = this;
}

ParticipantImpl::~ParticipantImpl()
{
	// TODO Auto-generated destructor stub
}

const GUID_t& ParticipantImpl::getGuid() const
{
	return this->mp_rtpsParticipant->getGuid();
}

Publisher* ParticipantImpl::createPublisher(PublisherAttributes& att,
		PublisherListener* listen)
{
	const char* const METHOD_NAME = "createPublisher";
	logInfo(PARTICIPANT,"CREATING PUBLISHER",C_B_YELLOW)
	//Look for the correct type registration

	TopicDataType* p_type = nullptr;

	if(!getRegisteredType(att.topic.getTopicDataType().c_str(),&p_type))
	{
		logError(PARTICIPANT,"Type : "<< att.topic.getTopicDataType() << " Not Registered");
		return nullptr;
	}
	if(att.topic.topicKind == WITH_KEY && !p_type->m_isGetKeyDefined)
	{
		logError(PARTICIPANT,"Keyed Topic needs getKey function");
		return nullptr;
	}
	if(m_att.rtps.builtin.use_STATIC_EndpointDiscoveryProtocol)
	{
		if(att.getUserDefinedID() <= 0)
		{
			logError(PARTICIPANT,"Static EDP requires user defined Id");
			return nullptr;
		}
	}
	if(!att.qos.checkQos() || !att.topic.checkQos())
		return nullptr;

	PublisherImpl* pubimpl = new PublisherImpl(this,p_type,att,listen);
	Publisher* pub = new Publisher(pubimpl);

	WriterAttributes watt;
	watt.endpoint.durabilityKind = att.qos.m_durability.kind == VOLATILE_DURABILITY_QOS ? VOLATILE : TRANSIENT_LOCAL;
	watt.endpoint.endpointKind = WRITER;
	watt.endpoint.multicastLocatorList = att.multicastLocatorList;
	watt.endpoint.reliabilityKind = att.qos.m_reliability.kind == RELIABLE_RELIABILITY_QOS ? RELIABLE : BEST_EFFORT;
	watt.endpoint.topicKind = att.topic.topicKind;
	watt.endpoint.unicastLocatorList = att.unicastLocatorList;
	watt.endpoint.setEntityID(att.getEntityID());
	watt.endpoint.setUserDefinedID(att.getUserDefinedID());
	watt.times = att.times;

	RTPSWriter* writer = RTPSDomain::createRTPSWriter(this->mp_rtpsParticipant,
												watt,
												(WriterHistory*)&pubimpl->m_history,
												(WriterListener*)&pubimpl->m_writerListener);
	if(writer == nullptr)
	{
		logError(PARTICIPANT,"Problem creating associated Writer");
		return nullptr;
	}
	pubimpl->mp_writer = writer;
	//SAVE THE PUBLICHER PAIR
	t_p_PublisherPair pubpair;
	pubpair.first = pub;
	pubpair.second = pubimpl;
	m_publishers.push_back(pubpair);

	//REGISTER THE WRITER
	//this->mp_rtpsParticipant->registerWriter(writer,att.topic,att.qos);

	return pub;
};

Subscriber* ParticipantImpl::createSubscriber(SubscriberAttributes& att,
		SubscriberListener* listen)
{
	const char* const METHOD_NAME = "createSubscriber";
	logInfo(PARTICIPANT,"CREATING SUBSCRIBER",C_B_YELLOW)
	//Look for the correct type registration

	TopicDataType* p_type = nullptr;

	if(!getRegisteredType(att.topic.getTopicDataType().c_str(),&p_type))
	{
		logError(PARTICIPANT,"Type : "<< att.topic.getTopicDataType() << " Not Registered");
		return nullptr;
	}
	if(att.topic.topicKind == WITH_KEY && !p_type->m_isGetKeyDefined)
	{
		logError(PARTICIPANT,"Keyed Topic needs getKey function");
		return nullptr;
	}
	if(m_att.rtps.builtin.use_STATIC_EndpointDiscoveryProtocol)
	{
		if(att.getUserDefinedID() <= 0)
		{
			logError(PARTICIPANT,"Static EDP requires user defined Id");
			return nullptr;
		}
	}
	if(!att.qos.checkQos() || !att.topic.checkQos())
		return nullptr;

	SubscriberImpl* subimpl = new SubscriberImpl(this,p_type,att,listen);
	Subscriber* sub = new Subscriber(subimpl);

	ReaderAttributes ratt;
	ratt.endpoint.durabilityKind = att.qos.m_durability.kind == VOLATILE_DURABILITY_QOS ? VOLATILE : TRANSIENT_LOCAL;
	ratt.endpoint.endpointKind = READER;
	ratt.endpoint.multicastLocatorList = att.multicastLocatorList;
	ratt.endpoint.reliabilityKind = att.qos.m_reliability.kind == RELIABLE_RELIABILITY_QOS ? RELIABLE : BEST_EFFORT;
	ratt.endpoint.topicKind = att.topic.topicKind;
	ratt.endpoint.unicastLocatorList = att.unicastLocatorList;
	ratt.endpoint.setEntityID(att.getEntityID());
	ratt.endpoint.setUserDefinedID(att.getUserDefinedID());
	ratt.times = att.times;

	RTPSReader* reader = RTPSDomain::createRTPSReader(this->mp_rtpsParticipant,
												ratt,
												(ReaderHistory*)&subimpl->m_history,
												(ReaderListener*)&subimpl->m_readerListener);
	if(reader == nullptr)
	{
		logError(PARTICIPANT,"Problem creating associated Reader");
		delete(subimpl);
		delete(sub);
		return nullptr;
	}
	subimpl->mp_reader = reader;
	//SAVE THE PUBLICHER PAIR
	t_p_SubscriberPair subpair;
	subpair.first = sub;
	subpair.second = subimpl;
	m_subscribers.push_back(subpair);

	//REGISTER THE READER
	//this->mp_rtpsParticipant->registerReader(writer,att.topic,att.qos);

	return sub;
};


bool ParticipantImpl::getRegisteredType(const char* typeName, TopicDataType** type)
{
	for(std::vector<TopicDataType*>::iterator it=m_types.begin();
			it!=m_types.end();++it)
	{
		if(strcmp((*it)->getName(),typeName)==0)
		{
			*type = *it;
			return true;
		}
	}
	return false;
}

bool ParticipantImpl::registerType(TopicDataType* type)
{
	const char* const METHOD_NAME = "registerType";
	if (type->m_typeSize <= 0)
	{
		logError(PARTICIPANT, "Registered Type must have maximum byte size > 0");
		return false;
	}
	if (type->m_typeSize > PAYLOAD_MAX_SIZE)
	{
		logError(PARTICIPANT, "Current version only supports types of sizes < " << PAYLOAD_MAX_SIZE);
		return false;
	}
	if (std::string(type->getName()).size() <= 0)
	{
		logError(PARTICIPANT, "Registered Type must have a name");
		return false;
	}
	for (auto ty = m_types.begin(); ty != m_types.end();++ty)
	{
		if (strcmp((*ty)->getName(), type->getName()) == 0)
		{
			logError(PARTICIPANT, "Type with the same name already exists:" << type->getName());
			return false;
		}
	}
	m_types.push_back(type);
	logInfo(PARTICIPANT, "Type " << type->getName() << " registered.");
	return true;
}


void ParticipantImpl::MyRTPSParticipantListener::onRTPSParticipantDiscovery(RTPSParticipant* part,RTPSParticipantDiscoveryInfo rtpsinfo)
{
	if(this->mp_participantimpl->mp_listener!=nullptr)
	{
		ParticipantDiscoveryInfo info;
		info.rtps = rtpsinfo;
		this->mp_participantimpl->mp_listener->onParticipantDiscovery(mp_participantimpl->mp_participant,info);
	}
}

} /* namespace pubsub */
} /* namespace eprosima */
