/*
 * Copyright 2010, Object Management Group, Inc.
 * Copyright 2010, PrismTech, Corp.
 * Copyright 2010, Real-Time Innovations, Inc.
 * Copyright 2019, Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OMG_DDS_CORE_XTYPES_DYNAMIC_TYPE_HPP_
#define OMG_DDS_CORE_XTYPES_DYNAMIC_TYPE_HPP_

#include <dds/core/xtypes/detail/DynamicType.hpp>

#include <dds/core/Reference.hpp>
#include <dds/core/xtypes/TypeKind.hpp>
#include <dds/core/xtypes/Annotation.hpp>

namespace dds {
namespace core {
namespace xtypes {

template<typename DELEGATE>
class TDynamicType : public Reference<DELEGATE>
{
    OMG_DDS_REF_TYPE_PROTECTED_DC(
            TDynamicType,
            Reference,
            DELEGATE)

public:
    const std::string& name() const
    {
        throw "Not implemented";
    }

    TypeKind kind() const
    {
        throw "Not implemented";
    }

    const std::vector<Annotation>& annotations() const
    {
        throw "Not implemented";
    }

    bool operator ==(
            const TDynamicType& that) const
    {
        throw "Not implemented";
    }

    bool operator !=(
            const TDynamicType& that) const
    {
        return !(*this == that);
    }

protected:
    TDynamicType(
            const std::string& name,
            TypeKind kind)
    {
        throw "Not implemented";
    }

    TDynamicType(
            const std::string& name,
            TypeKind kind,
            const Annotation& annotation)
    {
        throw "Not implemented";
    }

    TDynamicType(
            const std::string& name,
            TypeKind kind,
            const std::vector<Annotation>& annotations)
    {
        throw "Not implemented";
    }

    template<typename AnnotationIter>
    TDynamicType(
            const std::string& name,
            TypeKind kind,
            const AnnotationIter& begin,
            const AnnotationIter& end)
    {
        throw "Not implemented";
    }

    TDynamicType(
            const TDynamicType& other) = default;
};

template<typename T>
bool is_primitive_type(
        const TDynamicType<T>& t)
{
    throw "Not implemented";
}

template<typename T>
bool is_constructed_type(
        const TDynamicType<T>& t)
{
    throw "Not implemented";
}

template<typename T>
bool is_collection_type(
        const TDynamicType<T>& t)
{
    throw "Not implemented";
}

template<typename T>
bool is_aggregation_type(
        const TDynamicType<T>& t)
{
    throw "Not implemented";
}

typedef TDynamicType<detail::DynamicType> DynamicType;

} //namespace xtypes
} //namespace core
} //namespace dds

#endif //OMG_DDS_CORE_XTYPES_DYNAMIC_TYPE_HPP_