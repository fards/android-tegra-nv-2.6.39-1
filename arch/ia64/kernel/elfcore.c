#include <linux/elf.h>
#include <linux/coredump.h>
#include <linux/fs.h>
#include <linux/mm.h>

#include <asm/elf.h>


Elf64_Half elf_core_extra_phdrs(void)
{
	return GATE_EHDR->e_phnum;
}

int elf_core_write_extra_phdrs(struct file *file, loff_t offset, size_t *size,
			       unsigned long limit)
{
	const struct elf_phdr *const gate_phdrs =
		(const struct elf_phdr *) (GATE_ADDR + GATE_EHDR->e_phoff);
	int i;
	Elf64_Off ofs = 0;

	for (i = 0; i < GATE_EHDR->e_phnum; ++i) {
		struct elf_phdr phdr = gate_phdrs[i];

		if (phdr.p_type == PT_LOAD) {
			phdr.p_memsz = PAGE_ALIGN(phdr.p_memsz);
			phdr.p_filesz = phdr.p_memsz;
			if (ofs == 0) {
				ofs = phdr.p_offset = offset;
				offset += phdr.p_filesz;
			} else {
				phdr.p_offset = ofs;
			}
		} else {
			phdr.p_offset += ofs;
		}
		phdr.p_paddr = 0; /* match other core phdrs */
		*size += sizeof(phdr);
		if (*size > limit || !dump_write(file, &phdr, sizeof(phdr)))
			return 0;
	}
	return 1;
}

int elf_core_write_extra_data(struct file *file, size_t *size,
			      unsigned long limit)
{
	const struct elf_phdr *const gate_phdrs =
		(const struct elf_phdr *) (GATE_ADDR + GATE_EHDR->e_phoff);
	int i;

	for (i = 0; i < GATE_EHDR->e_phnum; ++i) {
		if (gate_phdrs[i].p_type == PT_LOAD) {
			void *addr = (void *)gate_phdrs[i].p_vaddr;
			size_t memsz = PAGE_ALIGN(gate_phdrs[i].p_memsz);

			*size += memsz;
			if (*size > limit || !dump_write(file, addr, memsz))
				return 0;
			break;
		}
	}
	return 1;
}