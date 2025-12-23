using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApp2
{
    public partial class Form1 : Form
    {
        private string chosenOutputFile = string.Empty;

        public Form1()
        {
            InitializeComponent();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "All files (*.*)|*.*|Text files (*.txt)|*.txt";
            if (openFileDialog1.ShowDialog() != DialogResult.OK) return;

            try
            {
                if (useEncryptionToolStripMenuItem.Checked)
                {
                    var bytes = File.ReadAllBytes(openFileDialog1.FileName);
                    string key = keyToolStripTextBox.Text;
                    if (string.IsNullOrEmpty(key))
                    {
                        MessageBox.Show("Please enter an encryption key in the Encryption menu.", "Key required", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        return;
                    }

                    textBoxData.Text = DecryptBytes(bytes, key);
                }
                else
                {
                    textBoxData.Text = File.ReadAllText(openFileDialog1.FileName, Encoding.UTF8);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error reading file: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (!string.IsNullOrEmpty(chosenOutputFile))
            {
                saveFileDialog1.FileName = Path.GetFileName(chosenOutputFile);
                saveFileDialog1.InitialDirectory = Path.GetDirectoryName(chosenOutputFile);
            }

            if (saveFileDialog1.ShowDialog() != DialogResult.OK) return;

            try
            {
                if (useEncryptionToolStripMenuItem.Checked)
                {
                    string key = keyToolStripTextBox.Text;
                    if (string.IsNullOrEmpty(key))
                    {
                        MessageBox.Show("Please enter an encryption key in the Encryption menu.", "Key required", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        return;
                    }

                    byte[] encrypted = EncryptString(textBoxData.Text, key);
                    File.WriteAllBytes(saveFileDialog1.FileName, encrypted);
                }
                else
                {
                    File.WriteAllText(saveFileDialog1.FileName, textBoxData.Text, Encoding.UTF8);
                }

                chosenOutputFile = saveFileDialog1.FileName;
                MessageBox.Show("Saved to " + chosenOutputFile, "Saved", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error saving file: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void chooseOutputToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (saveFileDialog1.ShowDialog() != DialogResult.OK) return;
            chosenOutputFile = saveFileDialog1.FileName;
            MessageBox.Show("Chosen output file: " + chosenOutputFile, "Output file", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void aboutProgramToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string help = "Simple program to open, view, save and optionally encrypt/decrypt files.\n\n" +
                          "Use File->Open to load a file, File->Save to write the TextBox contents.\n" +
                          "Use Encryption->Use Encryption and enter a key in the menu to enable encryption/decryption.";
            MessageBox.Show(help, "About", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        // ---------- Encryption helpers (AES) ----------
        private static byte[] EncryptString(string plainText, string password)
        {
            using (var aes = Aes.Create())
            {
                var salt = Encoding.UTF8.GetBytes("fixed_salt_12345");
                var key = new Rfc2898DeriveBytes(password, salt, 10000);
                aes.Key = key.GetBytes(32);
                aes.IV = key.GetBytes(16);
                using (var ms = new MemoryStream())
                {
                    using (var cs = new CryptoStream(ms, aes.CreateEncryptor(), CryptoStreamMode.Write))
                    using (var sw = new StreamWriter(cs, Encoding.UTF8))
                    {
                        sw.Write(plainText);
                    }
                    return ms.ToArray();
                }
            }
        }

        private static string DecryptBytes(byte[] cipher, string password)
        {
            using (var aes = Aes.Create())
            {
                var salt = Encoding.UTF8.GetBytes("fixed_salt_12345");
                var key = new Rfc2898DeriveBytes(password, salt, 10000);
                aes.Key = key.GetBytes(32);
                aes.IV = key.GetBytes(16);
                using (var ms = new MemoryStream(cipher))
                using (var cs = new CryptoStream(ms, aes.CreateDecryptor(), CryptoStreamMode.Read))
                using (var sr = new StreamReader(cs, Encoding.UTF8))
                {
                    return sr.ReadToEnd();
                }
            }
        }

        private void textBoxData_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
