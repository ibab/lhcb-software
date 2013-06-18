#define DIMLIB
#include "tokenstring.hxx"

TokenString::TokenString(char *str)
{
	token_buff = new char[((int)strlen(str)+1)*2];
	token_ptr = token_buff;
	token_seps = 0;
	store_str(str);
	token_ptr = token_buff;
	curr_token_ptr = token_ptr;
}

TokenString::TokenString(char *str, char *seps)
{
	token_buff = new char[((int)strlen(str)+1)*2];
	token_ptr = token_buff;
	token_seps = new char[((int)strlen(seps)+1)];
	strcpy(token_seps,seps);
	store_str(str);
	token_ptr = token_buff;
	curr_token_ptr = token_ptr;
}

TokenString::~TokenString()
{
  if(token_buff) 
  { 
	  delete []token_buff; 
	  token_buff = 0; 
  }
  if(token_seps) 
  { 
	  delete []token_seps; 
	  token_seps = 0; 
  }
}

void TokenString::store_str(char *str)
{
int i, in_tok = 0;
int sep = 0;

	n_tokens = 0;
	if(!token_seps)
	{
		while(*str)
		{
			if( (*str == '@') || (*str == '|') || (*str == '/') || 
				(*str == '=') || (*str == '(') || (*str == ')') ||
				(*str == '.') || (*str == '\n'))
			{   	
				if(in_tok)
				{
					*token_ptr++ = '\0';
					n_tokens++;
				}
				*token_ptr++ = *str++;
				*token_ptr++ = '\0';
				n_tokens++;
				in_tok = 0;
			}
			else if(*str == '"')
			{
				if(in_tok)
				{
					*token_ptr++ = '\0';
					n_tokens++;
				}
				*token_ptr++ = *str++;
				while(*str != '"')
				{
					*token_ptr++ = *str++;
				}
				*token_ptr++ = *str++;
				*token_ptr++ = '\0';
				n_tokens++;
				in_tok = 0;
			}
			else if(*str == ':')
			{
				if(*(str+1) == ':')
				{
					if(in_tok)
					{
						*token_ptr++ = '\0';
						n_tokens++;
					}
					*token_ptr++ = *str++;
					*token_ptr++ = *str++;
					*token_ptr++ = '\0';
					n_tokens++;
					in_tok = 0;
				}
				else
				{
					*token_ptr++ = *str++;
					in_tok = 1;
				}
			}
			else
			{
				*token_ptr++ = *str++;
				in_tok = 1;
			}
		}
	}
	else
	{
		while(*str)
		{
			sep = 0;
			for(i = 0; i < (int)strlen(token_seps); i++)
			{
				if(*str == token_seps[i])
				{
					if(in_tok)
					{
						*token_ptr++ = '\0';
						n_tokens++;
					}
					*token_ptr++ = *str++;
					*token_ptr++ = '\0';
					sep = 1;
					in_tok = 0;
					n_tokens++;
					break;
				}
			}
			if(!sep)
			{
				*token_ptr++ = *str++;
				in_tok = 1;
			}
		}
	}
	if(in_tok)
	{
		*token_ptr++ = '\0';
		n_tokens++;
	}
	*token_ptr++ = '\0';
}

int TokenString::getToken(char *&token)
{

	if(!*token_ptr)
	{
		token_ptr = token_buff;
		curr_token_ptr = token_ptr;
		return(0);
	}

	curr_token_ptr = token_ptr;
    token_ptr += (int)strlen(curr_token_ptr)+1;
	token = curr_token_ptr;

	return(1);
}		

void TokenString::pushToken()
{
	push_token_ptr = token_ptr;
}

void TokenString::popToken()
{
	token_ptr = push_token_ptr;
}

int TokenString::cmpToken(char *str)
{
	if(!strcmp(curr_token_ptr, str))
		return(1);
	return(0);
}

int TokenString::firstToken()
{
	if(curr_token_ptr == token_buff)
		return(1);
	return(0);
}

int TokenString::getNTokens()
{
	return n_tokens;
}

int TokenString::getNTokens(char *str)
{
	int n = 0;
	char *token;

	while(getToken(token))
	{
		if(!strcmp(token,str))
			n++;
	}
	return n;
}
/*
main(int argc, char *argv[])
{
	TokenString *ts;
	char *token;

	ts = new TokenString(argv[1],"/)");
	cout << "n = " << ts->getNTokens() << "\n";
	cout.flush();
	while(ts->getToken(token))
	{
		cout << token << "\n";
		cout.flush();
	}
}
*/
