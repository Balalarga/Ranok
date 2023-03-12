#include "BoundingView.h"

#include <glm/glm.hpp>

#include "Graphics/Materials/BaseMaterial.h"

glm::fvec3 vertices[] =
{
	{-1,  1, -1},
	{-1,  1,  1},
	{-1,  1,  1},
	{ 1,  1,  1},
	{ 1,  1,  1},
	{ 1,  1, -1},
	{ 1,  1, -1},
	{-1,  1, -1},
	{-1, -1, -1},
	{-1, -1,  1},
	{-1, -1,  1},
	{ 1, -1,  1},
	{ 1, -1,  1},
	{ 1, -1, -1},
	{ 1, -1, -1},
	{-1, -1, -1},
	{-1,  1,  1},
	{-1, -1,  1},
	{ 1,  1,  1},
	{ 1, -1, -1},
	{-1,  1, -1},
	{ 1, -1,  1},
	{ 1,  1, -1},
	{-1, -1, -1},
};

Buffer bufferInfo(DataPtr(vertices, std::size(vertices), sizeof(vertices[0])), BufferLayout().Float(3), GL_LINES);

BoundingView::BoundingView():
	Object(bufferInfo)
{
	
}
