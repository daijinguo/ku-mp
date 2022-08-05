#include "libcommon/any.h"

KU_NS_LEFT
namespace common
{

any::any()
{
}

any::~any()
{
}

class AAA : public any {
public:
	using ptr = std::shared_ptr<AAA>;

	ptr shared_from_this()
	{
		return any::shared_from_this(this);
	}
};

} // namespace common
KU_NS_RIGHT
