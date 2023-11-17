using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace AlphaTraining
{
    public class UserCard
    {
        private string _name = string.Empty;
        private string _surname = string.Empty;
        private int _age = 0;
        private string _gender = "undefined";
        private string _id;

        public string Name
        {
            get
            {
                return _name;
            }

            set
            {
                _name = value;
            }
        }

        public string Surname
        {
            get
            {
                return _surname;
            }
                
            set
            {
                _surname = value;
            }
        }

        public string Gender
        {
            get
            {
                return _gender;
            }

            set
            {
                _gender = value;
            }
        }

        public int Age
        {
            get
            {
                return _age;
            }
            
            set
            {
                _age = value;
            }
        }

        public string Id
        {
            set
            {
                _id = value;
            }

            get
            {
                return _id;
            }
        }

        public UserCard()
        {
        }

        

        public static UserCard Load(string userId)
        {
            string cardPath = Path.Combine(@".\Data\users", userId, "UserCard.json");
            UserCard userCard = null;

            using (StreamReader sr = new StreamReader(File.OpenRead(cardPath)))
            {
                try
                {
                    userCard = JsonSerializer.Deserialize<UserCard>(sr.ReadToEnd());
                }
                catch(JsonException)
                {                    
                }
            }
                        
            return userCard;
        }


        public string GenerateId()
        {
            string id_main = Transliteration.ToEnglish(
                Name.Substring(0, Math.Min(_name.Length, 3)))
                + Transliteration.ToEnglish(Surname.Substring(0, Math.Min(_surname.Length, 3)));


            // Проверить, есть ли пользователи с данным именем
            int nNumber = 0;
            bool isUnique = true;

            do
            {
                isUnique = true;

                // Перечислить каталоги в папке с пользователями
                foreach (var filename in Directory.GetDirectories(@".\Data\users"))
                {
                    if ((id_main + nNumber.ToString()) == Path.GetFileName(filename))
                    {
                        nNumber++;
                        isUnique = false;
                        break;
                    }
                }

            } while (false == isUnique);

            _id = id_main + nNumber.ToString();

            return _id;
        }

        public void Serialize()
        {
            string cardPath = Path.Combine(@".\Data\users", _id, "UserCard.json");

            using (StreamWriter sw = new StreamWriter(File.OpenWrite(cardPath)))
            {
                var options = new JsonSerializerOptions
                {
                    WriteIndented = true
                };

                sw.Write(JsonSerializer.Serialize(this, options));
            }
        }

    }
}
