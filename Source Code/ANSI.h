#pragma once

#define ANSI_ESC "\u001b"
#define ANSI_CSI ANSI_ESC"["
#define ANSI_RESET_ALL ANSI_CSI"0m"

#define ANSI_ERR_COLOR ANSI_CSI"38;5;196m"
#define ANSI_INFO_COLOR ANSI_CSI"38;5;42m"
#define ANSI_DEBUG_COLOR ANSI_CSI"38;5;184m"
