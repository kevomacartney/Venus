//
// Created by Kelvin Macartney on 14/04/2020.
//

#ifndef VENUS_GENERICOBJECT_H
#define VENUS_GENERICOBJECT_H

#include <memory>

namespace Venus {
    /**
     * A class that can store any general type safely extracting the proper type from the internal data.
     */
    class GenericObject {
    private:

        /** Base class for data*/
        class DataBase {
        public:
            virtual ~DataBase() = default;

            [[nodiscard]] virtual std::shared_ptr<DataBase> clone() const = 0;
        };

        /** Represents the data inside a Type */
        template<typename ValueType>
        class Data : public DataBase {
        public:
            explicit Data(const ValueType &value)
                    : value(value) {}


            [[nodiscard]]
            std::shared_ptr<DataBase> clone() const override {
                return std::make_shared<Data>(value);
            }

            ValueType value;
        };

    public:
        GenericObject() = default;

        template<typename ValueType>
        explicit GenericObject(const ValueType &value):
                _typeData(std::make_shared<Data<ValueType>>(value)) {}

        GenericObject(const GenericObject &otherType) :
                _typeData(otherType._typeData != nullptr ? otherType._typeData->clone() : nullptr) {}

        GenericObject &swap(GenericObject &rhs) {
            std::swap(this->_typeData, rhs._typeData);
            return *this;
        }

        /** Returns true if no type is set. */
        bool empty() const {
            return this->_typeData == nullptr;
        }

        /** Returns the value store by the generic type */
        template<typename ValueType>
        ValueType getValue() {
            return *GenericObject::any_cast<ValueType>(this);
        }

        /** Returns the pointer to the data store by the generic type */
        template<typename ValueType>
        ValueType* getValuePointer() {
            return GenericObject::any_cast<ValueType>(this);
        }

        ~ GenericObject() = default;

        template<typename ValueType>
        GenericObject &operator=(const ValueType &rhs) {
            Any(rhs).swap(*this);
            return *this;
        }

        GenericObject &operator=(const GenericObject &rhs) {
            GenericObject(rhs).swap(*this);
            return *this;
        }

        /** Returns a pointer to the internal data of the specified type. */
        template<typename ValueType>
        static ValueType *any_cast(GenericObject *operand) {
            if (operand != nullptr)
                return &static_cast<Data<ValueType> *>(operand->_typeData.get())->value;
            else
                return nullptr;
        }

    private:
        std::shared_ptr<DataBase> _typeData;
    };
}
#endif //VENUS_GENERICOBJECT_H
