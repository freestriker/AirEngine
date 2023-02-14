#pragma once
namespace AirEngine
{
	namespace Utility
	{
		template<typename T>
		class ChildBrotherTreeNode
		{
		public:
			struct Iterator
			{
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = T;
				using pointer = T*;
				using reference = T&;
			private:
				pointer _object;
			public:
				inline Iterator(reference object)
					: _object(&object)
				{
				}
				inline Iterator()
					: _object(nullptr)
				{
				}
				inline reference operator*() 
				{ 
					return *_object; 
				}
				inline pointer operator->() 
				{ 
					return _object; 
				}
				inline Iterator& operator++() 
				{ 
					_object = &static_cast<ChildBrotherTreeNode<T>*>(_object)->YoungerBrother();
					return *this;
				}
				inline Iterator operator++(int)
				{ 
					Iterator tmp = *this; 
					++(*this); 
					return tmp; 
				}
				inline Iterator& operator--()
				{ 
					_object = static_cast<ChildBrotherTreeNode<T>*>(_object)->ElderBrother();
					return *this;
				}
				inline Iterator operator--(int)
				{ 
					Iterator tmp = *this; 
					--(*this); 
					return tmp; 
				}
				inline operator bool() {
					return _object != nullptr;
				}
				friend inline bool operator== (const Iterator& a, const Iterator& b)
				{ 
					return a._object == b._object; 
				};
				friend inline bool operator!= (const Iterator& a, const Iterator& b)
				{ 
					return a._object != b._object; 
				}; 
			};
		private:
			ChildBrotherTreeNode<T>* _parentNode;
			ChildBrotherTreeNode<T>* _elderBrotherNode;
			ChildBrotherTreeNode<T>* _youngerBrotherNode;
			ChildBrotherTreeNode<T>* _childNode;
		protected:
			virtual void OnAttachToTree()
			{

			}
			virtual void OnDetachFromTree()
			{

			}
		public:
			inline ChildBrotherTreeNode();
			inline bool HaveParent() const;
			inline bool HaveElderBrother() const;
			inline bool HaveYoungerBrother() const;
			inline bool HaveChild() const;
			inline T& Parent();
			inline T& ElderBrother();
			inline T& YoungerBrother();
			inline T& Child();
			inline void AddChild(T& child);
			inline void AddElderBrother(T& newBrother);
			inline void AddEldestBrother(T& newBrother);
			inline void AddYoungerBrother(T& newBrother);
			inline void AddYoungestBrother(T& newBrother);
			inline void RemoveSelf();
			inline ChildBrotherTreeNode<T>::Iterator ChildIterator();
			inline ChildBrotherTreeNode<T>::Iterator BrotherIterator();
			static inline ChildBrotherTreeNode<T>::Iterator UnvalidIterator();
		};
		template<typename T>
		inline ChildBrotherTreeNode<T>::ChildBrotherTreeNode()
			: _parentNode(nullptr)
			, _youngerBrotherNode(nullptr)
			, _elderBrotherNode(nullptr)
			, _childNode(nullptr)
		{
		}
		template<typename T>
		inline bool ChildBrotherTreeNode<T>::HaveParent() const
		{
			return _parentNode != nullptr;
		}
		template<typename T>
		inline bool ChildBrotherTreeNode<T>::HaveElderBrother() const
		{
			return _parentNode != nullptr;
		}
		template<typename T>
		inline bool ChildBrotherTreeNode<T>::HaveYoungerBrother() const
		{
			return _youngerBrotherNode != nullptr;
		}
		template<typename T>
		inline bool ChildBrotherTreeNode<T>::HaveChild() const
		{
			return _childNode != nullptr;
		}
		template<typename T>
		inline T& ChildBrotherTreeNode<T>::Parent()
		{
			return static_cast<T&>(*_parentNode);
		}
		template<typename T>
		inline T& ChildBrotherTreeNode<T>::YoungerBrother()
		{
			return static_cast<T&>(*_youngerBrotherNode);
		}
		template<typename T>
		inline T& ChildBrotherTreeNode<T>::ElderBrother()
		{
			return static_cast<T&>(*_elderBrotherNode);
		}
		template<typename T>
		inline T& ChildBrotherTreeNode<T>::Child()
		{
			return static_cast<T&>(*_childNode);
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::AddChild(T& newChild)
		{
			ChildBrotherTreeNode<T>& newChildNode = static_cast<ChildBrotherTreeNode<T>&>(newChild);

			if (_childNode)
			{
				_childNode->AddYoungestBrother(newChild);
			}
			else
			{
				newChildNode._parentNode = this;
				newChildNode._youngerBrotherNode = nullptr;
				newChildNode._elderBrotherNode = nullptr;
				_childNode = &newChild;

				newChildNode.OnAttachToTree();
			}
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::AddElderBrother(T& newBrother)
		{
			ChildBrotherTreeNode<T>& newBrotherNode = static_cast<ChildBrotherTreeNode<T>&>(newBrother);

			newBrotherNode._parentNode = _parentNode;
			if (_elderBrotherNode)
			{
				_elderBrotherNode->_youngerBrotherNode = &newBrotherNode;
				newBrotherNode._youngerBrotherNode = this;
				newBrotherNode._elderBrotherNode = _elderBrotherNode;
				_elderBrotherNode = &newBrotherNode;
			}
			else
			{
				_parentNode->_childNode = &newBrotherNode;
				newBrotherNode._elderBrotherNode = nullptr;
				newBrotherNode._youngerBrotherNode = this;
				_elderBrotherNode = &newBrotherNode;
			}

			newBrotherNode.OnAttachToTree();
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::AddEldestBrother(T& newBrother)
		{
			ChildBrotherTreeNode<T>& newBrotherNode = static_cast<ChildBrotherTreeNode<T>&>(newBrother);

			newBrotherNode._parentNode = _parentNode;

			newBrotherNode._youngerBrotherNode = _parentNode->_childNode;
			_parentNode->_childNode->_elderBrotherNode = &newBrotherNode;
			_parentNode->_childNode = &newBrotherNode;
			newBrotherNode._elderBrotherNode = nullptr;

			newBrotherNode.OnAttachToTree();
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::AddYoungerBrother(T& newBrother)
		{
			ChildBrotherTreeNode<T>& newBrotherNode = static_cast<ChildBrotherTreeNode<T>&>(newBrother);

			newBrotherNode._parentNode = _parentNode;
			if (_youngerBrotherNode)
			{
				_youngerBrotherNode->_elderBrotherNode = &newBrotherNode;
				newBrotherNode._elderBrotherNode = this;
				newBrotherNode._youngerBrotherNode = _youngerBrotherNode;
				_youngerBrotherNode = &newBrotherNode;
			}
			else
			{
				newBrotherNode._elderBrotherNode = this;
				newBrotherNode._youngerBrotherNode = nullptr;
				_youngerBrotherNode = &newBrotherNode;
			}

			newBrotherNode.OnAttachToTree();
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::AddYoungestBrother(T& newBrother)
		{
			ChildBrotherTreeNode<T>& newBrotherNode = static_cast<ChildBrotherTreeNode<T>&>(newBrother);

			ChildBrotherTreeNode<T>* lastNode = this;
			while (lastNode->_youngerBrotherNode)
			{
				lastNode = lastNode->_youngerBrotherNode;
			}

			lastNode->_youngerBrotherNode = &newBrotherNode;
			newBrotherNode._parentNode = _parentNode;
			newBrotherNode._elderBrotherNode = lastNode;
			newBrotherNode._youngerBrotherNode = nullptr;

			newBrotherNode.OnAttachToTree();
		}
		template<typename T>
		inline void ChildBrotherTreeNode<T>::RemoveSelf()
		{
			OnDetachFromTree();

			if (_parentNode)
			{
				if (_elderBrotherNode)
				{
					if (_youngerBrotherNode)
					{
						_elderBrotherNode->_youngerBrotherNode = _youngerBrotherNode;
						_youngerBrotherNode->_elderBrotherNode = _elderBrotherNode;
					}
					else
					{
						_elderBrotherNode->_youngerBrotherNode = nullptr;
					}
				}
				else
				{
					if (_youngerBrotherNode)
					{
						_parentNode->_childNode = _youngerBrotherNode;
						_youngerBrotherNode->_elderBrotherNode = nullptr;
					}
					else
					{
						
					}
				}

				_parentNode = nullptr;
			}

			_elderBrotherNode = nullptr;
			_youngerBrotherNode = nullptr;
		}
		template<typename T>
		inline typename ChildBrotherTreeNode<T>::Iterator ChildBrotherTreeNode<T>::ChildIterator()
		{
			return ChildBrotherTreeNode<T>::Iterator(Child());
		}
		template<typename T>
		inline typename ChildBrotherTreeNode<T>::Iterator ChildBrotherTreeNode<T>::BrotherIterator()
		{
			return ChildBrotherTreeNode<T>::Iterator(_parentNode->Child());
		}
		template<typename T>
		inline typename ChildBrotherTreeNode<T>::Iterator ChildBrotherTreeNode<T>::UnvalidIterator()
		{
			return ChildBrotherTreeNode<T>::Iterator();
		}
	}
}