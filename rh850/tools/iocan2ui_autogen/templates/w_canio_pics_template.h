#ifndef _W_PICS_H_
#define _W_PICS_H_
{# DEFINES #}
{% for pic in pics %}
#define	_PIC_{{ pic[0] }}		_PIC_BASE + {{ pic[4] }}{% endfor %}

{% for pic in pics %}
#define	PIC_ID_{{ pic[0] }}			{{ loop.index0 }}{% endfor %}
{# end DEFINES #}

/* picture information */
typedef struct {
	uint16_t pid;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	void * address; 
	uint16_t length;
	uint8_t ram:1; 
	uint8_t :7; 
	
} pic_t;

extern pic_t pics[{{ pic_size }}];

void W_CANIO_PICS_Init(void);


#endif //_W_PICS_H_
