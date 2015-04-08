//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/Container/Vector.h>

namespace Prime
{
	namespace UI
	{
		// Resource picker functionality
		const unsigned int  ACTION_PICK = 1;
		const unsigned int  ACTION_OPEN = 2;
		const unsigned int  ACTION_EDIT = 4;
		const unsigned int  ACTION_TEST = 8;

		class ResourcePicker
		{
		private:

		public:
			ResourcePicker(const Urho3D::String& typeName_, const Urho3D::String& filter_, unsigned int  actions_ = ACTION_PICK | ACTION_OPEN)
			{
				typeName = typeName_;
				type = Urho3D::StringHash(typeName_);
				actions = actions_;
				filters.Push(filter_);
				filters.Push("*.*");
				lastFilter = 0;
			}

			ResourcePicker(const Urho3D::String& typeName_, const Urho3D::Vector<Urho3D::String>& filters_, unsigned int  actions_ = ACTION_PICK | ACTION_OPEN)
			{
				typeName = typeName_;
				type = Urho3D::StringHash(typeName_);
				filters = filters_;
				actions = actions_;
				filters.Push("*.*");
				lastFilter = 0;
			}

			Urho3D::String typeName;
			Urho3D::StringHash type;
			Urho3D::String lastPath;
			unsigned int  lastFilter;
			Urho3D::Vector<Urho3D::String> filters;
			unsigned int  actions;
		};

		// VariantVector decoding & editing for certain components
		class VectorStruct
		{
		private:

		public:
			VectorStruct(const Urho3D::String& componentTypeName_, const Urho3D::String& attributeName_, const Urho3D::Vector<Urho3D::String>& variableNames_, unsigned int restartIndex_)
			{
				componentTypeName = componentTypeName_;
				attributeName = attributeName_;
				variableNames = variableNames_;
				restartIndex = restartIndex_;
			}
			Urho3D::String componentTypeName;
			Urho3D::String attributeName;
			Urho3D::Vector<Urho3D::String> variableNames;
			unsigned int restartIndex;
		};



		class ResourcePickerManager : public Urho3D::Object
		{
			OBJECT(ResourcePickerManager);
		public:
			ResourcePickerManager(Urho3D::Context* context);
			virtual ~ResourcePickerManager();
			/// Register object factory.
			static void RegisterObject(Urho3D::Context* context);

			void Init();

			void Clear();
			VectorStruct* GetVectorStruct(Urho3D::Vector<Urho3D::Serializable*>& serializables, unsigned int index);
			VectorStruct* GetVectorStruct(Urho3D::Serializable* serializable, unsigned int index);

			ResourcePicker* GetResourcePicker(Urho3D::StringHash resourceType);

			Urho3D::Vector<Urho3D::Serializable*>& GetresourceTargets();
			ResourcePicker* GetCurrentResourcePicker();
			void SetCurrentResourcePicker(ResourcePicker* picker);

			unsigned int GetresourcePickIndex()
			{
				return resourcePickIndex;
			}
			unsigned int GetresourcePickSubIndex()
			{
				return resourcePickSubIndex;
			}
			void SetresourcePickSubIndex(unsigned int val)
			{
				resourcePickSubIndex = val;
			}
			void SetresourcePickIndex(unsigned int val)
			{
				resourcePickIndex = val;
			}
		private:


			void InitResourcePicker();

			void InitVectorStructs();

			bool initialized_;
			Urho3D::Vector<ResourcePicker*> resourcePickers;
			Urho3D::Vector<Urho3D::Serializable*> resourceTargets;
			unsigned int  resourcePickIndex;
			unsigned int  resourcePickSubIndex;
			ResourcePicker* resourcePicker;

			Urho3D::Vector<VectorStruct*> vectorStructs;
		};

	}
}