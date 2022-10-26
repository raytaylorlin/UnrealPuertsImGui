export function combineCombo(items: string[]) {
    return items.join('\0') + '\0';
}