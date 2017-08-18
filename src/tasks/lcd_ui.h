/*
 * lcd_ui.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TASK_LCD_UI_H
#define TASK_LCD_UI_H



void wdy_init_msg(const char *fmt, ...);

void UI_app_task(void);

#endif /* !TASK_LCD_UI_H */
