/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file CacheChange.h
 */

#ifndef CACHECHANGE_H_
#define CACHECHANGE_H_

#include "Types.h"
#include "WriteParams.h"
#include "SerializedPayload.h"
#include "Time_t.h"
#include "InstanceHandle.h"

namespace eprosima
{
    namespace fastrtps
    {
        namespace rtps
        {


            /**
             * @enum ChangeKind_t, different types of CacheChange_t.
             * @ingroup COMMON_MODULE
             */
#if defined(_WIN32)
            enum RTPS_DllAPI ChangeKind_t{
#else
                enum ChangeKind_t{
#endif
                    ALIVE,                //!< ALIVE
                    NOT_ALIVE_DISPOSED,   //!< NOT_ALIVE_DISPOSED
                    NOT_ALIVE_UNREGISTERED,//!< NOT_ALIVE_UNREGISTERED
                    NOT_ALIVE_DISPOSED_UNREGISTERED //!<NOT_ALIVE_DISPOSED_UNREGISTERED
                };


                /**
                 * Structure CacheChange_t, contains information on a specific CacheChange.
                 * @ingroup COMMON_MODULE
                 */
                struct RTPS_DllAPI CacheChange_t{
                    //!Kind of change, default value ALIVE.
                    ChangeKind_t kind;
                    //!GUID_t of the writer that generated this change.
                    GUID_t writerGUID;
                    //!Handle of the data associated wiht this change.
                    InstanceHandle_t instanceHandle;
                    //!SequenceNumber of the change
                    SequenceNumber_t sequenceNumber;
                    //!Serialized Payload associated with the change.
                    SerializedPayload_t serializedPayload;
                    //!Indicates if the cache has been read (only used in READERS)
                    bool isRead;
                    //!Source TimeStamp (only used in Readers)
                    Time_t sourceTimestamp;

                    WriteParams write_params;
                    bool is_untyped_;

                    //!Default constructor.
                    CacheChange_t():
                        kind(ALIVE),
                        isRead(false),
                        is_untyped_(true)
                    {

                    }

                    /**
                     * Constructor with payload size
                     * @param payload_size Serialized payload size
                     */
                    // TODO Check pass uint32_t to serializedPayload that needs int16_t.
                    CacheChange_t(uint32_t payload_size, bool is_untyped = false):
                        kind(ALIVE),
                        serializedPayload((uint16_t)payload_size),
                        isRead(false),
                        is_untyped_(is_untyped)
                    {

                    }
                    /*!
                     * Copy a different change into this one. All the elements are copied, included the data, allocating new memory.
                     * @param[in] ch_ptr Pointer to the change.
                     * @return True if correct.
                     */
                    bool copy(CacheChange_t* ch_ptr)
                    {
                        kind = ch_ptr->kind;
                        writerGUID = ch_ptr->writerGUID;
                        instanceHandle = ch_ptr->instanceHandle;
                        sequenceNumber = ch_ptr->sequenceNumber;
                        sourceTimestamp = ch_ptr->sourceTimestamp;
                        write_params = ch_ptr->write_params;
                        return serializedPayload.copy(&ch_ptr->serializedPayload, (ch_ptr->is_untyped_ ? false : true));
                    }
                    ~CacheChange_t(){

                    }
                };

#ifndef DOXYGEN_SHOULD_SKIP_THIS_PUBLIC

                /**
                 * Enum ChangeForReaderStatus_t, possible states for a CacheChange_t in a ReaderProxy.
                 *  @ingroup COMMON_MODULE
                 */
                enum ChangeForReaderStatus_t{
                    UNSENT = 0,        //!< UNSENT
                    UNACKNOWLEDGED = 1,//!< UNACKNOWLEDGED
                    REQUESTED = 2,     //!< REQUESTED
                    ACKNOWLEDGED = 3,  //!< ACKNOWLEDGED
                    UNDERWAY = 4       //!< UNDERWAY
                };
                /**
                 * Enum ChangeFromWriterStatus_t, possible states for a CacheChange_t in a WriterProxy.
                 *  @ingroup COMMON_MODULE
                 */
                enum ChangeFromWriterStatus_t{
                    UNKNOWN = 0,
                    MISSING = 1,
                    //REQUESTED_WITH_NACK,
                    RECEIVED = 2,
                    LOST = 3
                };



                /**
                 * Struct ChangeForReader_t used to represent the state of a specific change with respect to a specific reader, as well as its relevance.
                 *  @ingroup COMMON_MODULE
                 */
                class ChangeForReader_t
                {
                    friend struct ChangeForReaderCmp;

                    public:

                        ChangeForReader_t() : status_(UNSENT), is_relevant_(true),
                        change_(nullptr)
                        {
                        }

                        ChangeForReader_t(const ChangeForReader_t& ch) : status_(ch.status_),
                        is_relevant_(ch.is_relevant_), seq_num_(ch.seq_num_), change_(ch.change_)
                        {
                        }

                        ChangeForReader_t(const CacheChange_t* change) : status_(UNSENT),
                        is_relevant_(true), seq_num_(change->sequenceNumber), change_(change)
                        {
                        }

                        ChangeForReader_t(const SequenceNumber_t& seq_num) : status_(UNSENT),
                        is_relevant_(true), seq_num_(seq_num), change_(nullptr)
                        {
                        }

                        ~ChangeForReader_t(){}

                        ChangeForReader_t& operator=(const ChangeForReader_t& ch)
                        {
                            status_ = ch.status_;
                            is_relevant_ = ch.is_relevant_;
                            seq_num_ = ch.seq_num_;
                            change_ = ch.change_;
                            return *this;
                        }

                        /**
                         * Get the cache change
                         * @return Cache change
                         */
                        const CacheChange_t* getChange() const
                        {
                            return change_;
                        }

                        void setStatus(const ChangeForReaderStatus_t status)
                        {
                            status_ = status;
                        }

                        ChangeForReaderStatus_t getStatus() const
                        {
                            return status_;
                        }

                        void setRelevance(const bool relevance)
                        {
                            is_relevant_ = relevance;
                        }

                        bool isRelevant() const
                        {
                            return is_relevant_;
                        }

                        const SequenceNumber_t getSequenceNumber() const
                        {
                            return seq_num_;
                        }
                        
                        /**
                         * Set the cache change
                         * @param a_change Cache change
                         */
                        //TODO Remove
                        /*bool setChange(const CacheChange_t* a_change)
                        {
                            m_isValid = true;
                            seq_num_ = a_change->sequenceNumber;
                            change = a_change;
                            return true;
                        }*/

                        //! Set change as not valid
                        void notValid()
                        {
                            is_relevant_ = false;
                            change_ = nullptr;
                        }

                        //! Set change as valid
                        bool isValid() const
                        {
                            return change_ != nullptr;
                        }

                    private:

                        //!Status
                        ChangeForReaderStatus_t status_;

                        //!Boolean specifying if this change is relevant
                        bool is_relevant_;

                        //!Sequence number
                        SequenceNumber_t seq_num_;

                        const CacheChange_t* change_;
                };

                struct ChangeForReaderCmp
                {
                    bool operator()(const ChangeForReader_t& a, const ChangeForReader_t& b)
                    {
                        return a.seq_num_ < b.seq_num_;
                    }
                };

                /**
                 * Struct ChangeFromWriter_t used to indicate the state of a specific change with respect to a specific writer, as well as its relevance.
                 *  @ingroup COMMON_MODULE
                 */
                class ChangeFromWriter_t
                {
                    public:
                        ChangeFromWriter_t():status(UNKNOWN),is_relevant(true),m_isValid(false),change(NULL)
                    {

                    }
                        virtual ~ChangeFromWriter_t(){};
                        //!Status
                        ChangeFromWriterStatus_t status;
                        //!Boolean specifying if this change is relevant
                        bool is_relevant;
                        //!Sequence number
                        SequenceNumber_t seqNum;
                        /**
                         * Get the cache change
                         * @return Cache change
                         */
                        CacheChange_t* getChange()
                        {
                            return change;
                        }
                        /**
                         * Set the cache change
                         * @param a_change Cache change
                         */
                        bool setChange(CacheChange_t* a_change)
                        {
                            m_isValid = true;
                            seqNum = a_change->sequenceNumber;
                            change = a_change;
                            return true;
                        }
                        //! Set change as not valid
                        void notValid()
                        {
                            is_relevant = false;
                            m_isValid = false;
                            change = NULL;
                        }
                        //! Set change as valid
                        bool isValid()
                        {
                            return m_isValid;
                        }
                    private:
                        bool m_isValid;
                        CacheChange_t* change;

                };

#endif


            }
        }
    }


#endif /* CACHECHANGE_H_ */
