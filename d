[1mdiff --git a/dmdwidget.cpp b/dmdwidget.cpp[m
[1mindex 6b3ee55..b255e91 100644[m
[1m--- a/dmdwidget.cpp[m
[1m+++ b/dmdwidget.cpp[m
[36m@@ -208,16 +208,18 @@[m [mvoid DMDWidget::captureDMD()[m
 	{[m
 		p.fillRect(QRect(0, 0, DMDWIDTH * 2, DMDHEIGHT * 2), Qt::black);[m
 		m_DMD_color_found = false;[m
[32m+[m		[32mmemset(m_previous_DMD, 0, sizeof(m_previous_DMD));[m[41m[m
 	}[m
 	else[m
 	{[m
 		// valid DMD address[m
[31m-		uint8_t rawDMD[128 * 32];[m
[32m+[m		[32muint8_t rawDMD[DMDWIDTH * DMDHEIGHT];[m[41m[m
 		ReadProcessMemory(m_FX3_process_handle, (void*)(ptr), rawDMD, sizeof(rawDMD), NULL);[m
 		[m
 		if (isGarbage(rawDMD))[m
 		{[m
 			memset(rawDMD, 0, sizeof(rawDMD));[m
[32m+[m			[32mmemset(m_previous_DMD, 0, sizeof(m_previous_DMD));[m[41m[m
 		}[m
 		else[m
 		{[m
[36m@@ -225,6 +227,11 @@[m [mvoid DMDWidget::captureDMD()[m
 				getDMDColor();[m
 		}[m
 [m
[32m+[m		[32mif (isEqual(rawDMD, m_previous_DMD))[m[41m[m
[32m+[m			[32mreturn;[m[41m[m
[32m+[m		[32melse[m[41m[m
[32m+[m			[32mmemcpy(m_previous_DMD, rawDMD, sizeof(rawDMD));[m[41m[m
[32m+[m[41m[m
 		if (isWilliamsDMD(rawDMD))[m
 		{[m
 			correctWilliamsDMD(rawDMD);[m
[36m@@ -238,9 +245,9 @@[m [mvoid DMDWidget::captureDMD()[m
 		// Fill DMD with colors[m
 		p.fillRect(QRect(0, 0, DMDWIDTH * 2, DMDHEIGHT * 2), Qt::black);[m
 		uint32_t bytepos = 0;[m
[31m-		for (int y = 0; y < 32; ++y)[m
[32m+[m		[32mfor (int y = 0; y < DMDHEIGHT; ++y)[m[41m[m
 		{[m
[31m-			for (int x = 0; x < 128; ++x)[m
[32m+[m			[32mfor (int x = 0; x < DMDWIDTH; ++x)[m[41m[m
 			{[m
 				uint8_t c = rawDMD[bytepos];[m
 				float col = c / 255.0f;[m
[1mdiff --git a/dmdwidget.h b/dmdwidget.h[m
[1mindex 6532cc1..38c5ac7 100644[m
[1m--- a/dmdwidget.h[m
[1m+++ b/dmdwidget.h[m
[36m@@ -51,6 +51,7 @@[m [mprivate:[m
 	float m_DMD_r = 1.0f;[m
 	float m_DMD_g = 1.0f;[m
 	float m_DMD_b = 1.0f;[m
[32m+[m	[32muint8_t m_previous_DMD[128 * 32];[m[41m[m
 [m
 	QLabel* m_DMD_label = nullptr;[m
 	QTimer* captureTimer = nullptr;[m
