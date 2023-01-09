#pragma once

#define NO_CONTRUCTOR(typeName) \
typeName() = delete; \
~typeName() = delete;

#define NO_COPY(typeName) \
typeName(const typeName&) = delete;\
typeName& operator=(const typeName&) = delete;

#define NO_MOVE(typeName) \
typeName(typeName&&) = delete;\
typeName& operator=(typeName&&) = delete;