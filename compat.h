/////////////////////
// Approximate Compatibility with Macro Interface
//
// 

#define GET_NAME(ch)		((ch)->get_name())
	
#define GET_HIT(ch)		((ch)->get_hit())
#define GET_NAT_HIT(ch)		((ch)->get_nat_hit())
#define GET_MAX_HIT GET_NAT_HIT

#define GET_MANA(ch)		((ch)->get_mana())
#define GET_NAT_MANA(ch)	((ch)->get_nat_mana())
#define GET_MAX_MANA GET_NAT_MANA

#define GET_MOVE(ch)		((ch)->get_move())
#define GET_NAT_MOVE(ch)	((ch)->get_nat_move())
#define GET_MAX_MOVE GET_NAT_MOVE
